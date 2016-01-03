/*
  Copyright (C) 2015 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <emacs-module.h>

int plugin_is_GPL_compatible;

static emacs_value
Fcall_closure(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value fn = args[0];
	return env->funcall(env, fn, 0, NULL);
}

static emacs_value
Ffile_open(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value file_name = args[0];

	ptrdiff_t len = 0;
	env->copy_string_contents(env, file_name, NULL, &len);

	char *file = malloc(len);
	env->copy_string_contents(env, file_name, file, &len);

	int flag;
	if (env->eq(env, args[1], env->intern(env, ":r"))) {
		flag = O_RDONLY;
	} else if (env->eq(env, args[1], env->intern(env, ":w"))) {
		flag = O_WRONLY | O_CREAT | O_TRUNC;
	} else if (env->eq(env, args[1], env->intern(env, ":rw"))) {
		flag = O_RDWR | O_CREAT;
	} else {
		return env->intern(env, "nil");
	}

	int mode = 0644;
	if (nargs == 3) {
		mode = (int)env->extract_integer(env, args[2]);
	}

	int fd = open(file, flag, mode);
	if (fd == -1) {
		perror("open");
		free(file);
		return env->intern(env, "nil");
	}

	free(file);
	return env->make_integer(env, (intmax_t)fd);
}

static emacs_value
Ffile_read(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	int fd = (int)env->extract_integer(env, args[0]);
	size_t size = (size_t)env->extract_integer(env, args[1]);

	char *buf = malloc(size);
	if (buf == NULL) {
		return env->intern(env, "nil");
	}

	ssize_t len = read(fd, buf, size);
	if (len == -1) {
		return env->intern(env, "nil");
	}

	return env->make_string(env, (const char*)buf, size);
}

static emacs_value
Ffile_write(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	int fd = (int)env->extract_integer(env, args[0]);

	ptrdiff_t len = 0;
	env->copy_string_contents(env, args[1], NULL, &len);

	char *buf = malloc(len);
	if (buf == NULL)
		return env->intern(env, "nil");

	env->copy_string_contents(env, args[1], buf, &len);

	ssize_t written = write(fd, buf, len-1);
	if (written == -1) {
		return env->intern(env, "nil");
	}

	return env->make_integer(env, (intmax_t)written);
}

static emacs_value
Ffile_close(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	int fd = env->extract_integer(env, args[0]);
	if (close(fd) != 0) {
		return env->intern(env, "nil");
	}

	return env->intern(env, "t");
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("module-test-call-closure", Fcall_closure, 1, 1, "Call closure", NULL);
	DEFUN("module-test-open", Ffile_open, 2, 3, "File open", NULL);
	DEFUN("module-test-read", Ffile_read, 2, 2, "Read file", NULL);
	DEFUN("module-test-write", Ffile_write, 2, 2, "Write file", NULL);
	DEFUN("module-test-close", Ffile_close, 1, 1, "Close file", NULL);
#undef DEFUN

	provide(env, "module-test-core");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
