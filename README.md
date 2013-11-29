utilities
=========

Utility functions

Cleanline:

Cleans leading, trailing, and newlines from null terminated string.
Flags can be ORed together to perform subsets of functions, ie (CLEAN\_BEGIN | CLEAN\_END) for whitespace cleaning but leaving the newline.
Returns pointer to the cleaned string.
If you are using CLEAN\_COPY, the returned pointer will be to malloc()ed memory and you will have to free() it.
Caveat: if you CLEAN\_END but not CLEAN\_NL, whitespace between the string and newline will remain. ie, ("abc \n ", CLEAN\_END) becomes "abc \n" and not "abc\n"

Splitbuf:

Given a buffer, split on tokens and return an allocated null terminated array of pointers.
If (flags & SPLITBUF\_COPY) then copy string leaving original intact, else alter original string.
If (flags & SPLITBUF\_GROUP) then multiple sep characters in a row will be treated as 1 ie splitbuf("a\::b\::c", ":") becomes "a", "b", "c" ..otherwise "a", "", "b", "", "c".
If (flags & SPLITBUF\_STRING) then the "sep" argument is treated as a full string to split by and not a list of characters.
If (flags & SPLITBUF\_IGNORECASE) then comparisons are case insensitive ie splitbuf("abc123xyz", "123") becomes "abc", "xyz".
If (flags & STRIPBUF\_STRIPENCLOSE) then enclose strings are stripped at beginning and end ie splitbuf('"Joe","Cool"', ",", "\"") becomes "Joe", "Cool".. otherwise ""Joe"", ""Cool"".
Return value is number of elements in array.
When caller is done with the answer, it must free(\*retmembers);
