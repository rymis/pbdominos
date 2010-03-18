BEGIN {
	print "const char* help_message =";
}

{
	printf("\t\"%s\\n\"\n", gensub("\\\"", "\\\\\\\"", "g", gensub("\\\\", "\\\\\\\\", "g", $0)));
}

END {
	print ";";
}
