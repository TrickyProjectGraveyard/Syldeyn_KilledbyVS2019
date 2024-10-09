// License:
// 
// Syldeyn
// Convert old MKL license files to Syldeyn format
// 
// 
// 
// 	(c) Jeroen P. Broks, 2024
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 24.10.09
// End License


/*
 * This little utility is only meant to set the old MKL data into a format that Syldeyn can understand.
 * The reason I coded it in C# is simply because MKL uses the deprecated GINI system, which I already replaced
 * with GINIE before really going to C++, and I don't want to bother writing a GINI parser for C++, and a GINIE parser
 * (mind the 'e' at the end, yet the getup is much different, trust me) for C++ I already have. Once all projects have been
 * completely converted, I won't really need this tool anymore, do I?
 */

foreach (var dir in args) {
	Console.WriteLine($"Processing {dir}");
}
