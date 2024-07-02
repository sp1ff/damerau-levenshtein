# Copyright (C) 2020-2021 Michael Herstine <sp1ff@pobox.com>

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see https://www.gnu.org/licenses/.

EXTRA_DIST = lw-smoke uk-smoke br-smoke lw-br uk-br br-br \
	lw-damerau uk-damerau br-damerau \
	issue-2 issue-2.txt \
	issue-3 issue-3.txt \
	test-data-5-6 test-data-5-6.txt \
	test-data-12-8 test-data-12-8.txt
AM_TESTS_ENVIRONMENT=        \
	srcdir=$(srcdir)     \
	builddir=$(builddir)
TESTS = lw-smoke uk-smoke br-smoke lw-br uk-br br-br lw-damerau uk-damerau \
	br-damerau issue-2 issue-3 test-data-5-6 test-data-12-8

timing-tests: check-am FORCE
	builddir=$(builddir) srcdir=$(srcdir) ./timing

FORCE: ;

.PHONY: timing-tests
