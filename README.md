This is the bison++ part of the project from the now archived 
http://code.google.com/p/flexpp-bisonpp. The original aim of that project was
to fix the original bison++ so that it compiled with gcc 4.5.

At the moment there are two warnings to fix:

 * Use mktemp instead of mkstemp
 * Incorrect format string
