var bookOfWoo = File.read("wooscript/wooscript.txt");

workspace = Tools.ngramFrequency(bookOfWoo.toString(), 3, 10);

function test() {
	
}
