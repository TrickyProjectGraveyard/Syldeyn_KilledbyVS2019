

/*
 * This little utility is only meant to set the old MKL data into a format that Syldeyn can understand.
 * The reason I coded it in C# is simply because MKL uses the deprecated GINI system, which I already replaced
 * with GINIE before really going to C++, and I don't want to bother writing a GINI parser for C++, and a GINIE parser
 * (mind the 'e' at the end, yet the getup is much different, trust me) for C++ I already have. Once all projects have been
 * completely converted, I won't really need this tool anymore, do I?
 */

foreach(var dir in args) {
	Console.WriteLine($"Processing {dir}");
}