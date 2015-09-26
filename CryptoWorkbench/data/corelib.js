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

Tools.Hash = Object.freeze({
	Md4: 0,
	Md5: 1,
	Sha1: 2,
	Sha224: 3,
	Sha256: 4,
	Sha384: 5,
	Sha512: 6,
	Sha3_224: 7,
	Sha3_256: 8,
	Sha3_384: 9,
	Sha3_512: 10
});

ByteArray.StringFormat = Object.freeze({
	Latin1: 0,
	Utf8: 1,
	Hex: 2,
	Base64: 3,
});

ByteArray.HexFormat = Object.freeze({
	Basic: 0,
	Spaces: 1,
	Columns: 2,
});
