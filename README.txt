/*
* Lennart Hautzer
* 10 100 5235
* COMP2404 A3
* All rights reserved.
*/

HOW TO TEST:

Navigate to the A3 directory with your preferred terminal application.
Execute the command:

>. make

This will construct the project in its entirety. The application will be in the
resultant "binaries" directory. The make command will also execute the program
when run. If you end the process, you may restart it from the root directory by
either executing:

>. ./binaries/A3

or simply run:

>. make

again.

Please remember that any script file will need to be in the "data"
sub-directory in order for the program to read it when given the command:

>. .read file Name In Data Directory Without Quotes Or Spaces

In order to test the application with valgrind, please run:

>. valgrind ./binaries/A3

from the A3 root directory.

If it becomes necessary to remove the existing database, either manually
deconstruct it or simply delete "A3.db" from the "data" sub-directory. It will
be re-created with the expected schema, and will remain empty until it is
populated by the REPL interface or a script. A basic test script called
"test.txt" has been provided in the "data" sub-directory. If you wish to access
the sample "beatles.db" database, simply rename it to "A3.db" and it will be loaded
into the program. You may also populate the empty A3.db database using the
insert_beatles_tracks_rev3.txt script which has been provided.

The following requirements have been met:

PR 0.1: Yes.
PR 0.2: Yes.
PR 0.3: Yes.
PR 0.4: Yes.
PR 0.5: Yes.
PR 0.6: Yes.
PR 0.7: Yes.
PR 0.8: Yes.
PR 0.9: Yes.
PR 0.10: Yes.

DR 3.1: Yes

SR 3.1: Yes
SR 3.2: Yes
SR 3.3: Yes
SR 3.4: Yes

FR 3.1: Yes
FR 3.2: Yes
FR 3.3: Yes
FR 3.4: Yes
FR 3.5: Yes
FR 3.6: Yes
FR 3.7: Yes
FR 3.8: Yes
FR 3.9: Yes
FR 3.10: Yes
FR 3.11: Yes
FR 3.12: Yes
FR 3.13: Yes
