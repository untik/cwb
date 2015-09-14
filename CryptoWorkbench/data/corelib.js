function hello()
{
	return "Wheeee";
}

Tools.rot13 = function(text) {
	return Tools.rotateAlphabet(text, 13);
}

Tools.frequencyList = function(objectList) {
	var result = "";
	for (var i = 0; i < objectList.length; i++) {
		var item = objectList[i];
		result += item.value + "  " + item.frequency + "\n";
	}
	return result;
}

Tools.frequencyGraph = function(objectList, param1, param2) {
	var scale = 1.0;
	if (typeof param1 === "number" && param1 > 0)
		scale = param1;
	else if (typeof param2 === "number" && param2 > 0)
		scale = param2;
	
	var letter = "="
	if (typeof param2 === "string")
		letter = param2;
	else if (typeof param1 === "string")
		letter = param1;
	
	var result = "";
	for (var i = 0; i < objectList.length; i++) {
		var item = objectList[i];
		result += item.value + "  " + letter.repeat(item.frequency * scale) + "\n";
	}
	return result;
}