var input = File.read("wooscript.txt");
var ngrams = Tools.ngramFrequency(input.toString(), 3, 18);
workspace = Tools.frequencyGraph(ngrams);


