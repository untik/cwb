var bookOfWoo = File.read("wooscript/wooscript.txt");
var ngrams = Tools.ngramFrequency(bookOfWoo.toString(), 3, 18);
workspace = Tools.frequencyGraph(ngrams);

