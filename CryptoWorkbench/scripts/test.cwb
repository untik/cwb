var input = new ByteArray("The quick brown fox jumps over the lazy dog");

function testHash()
{
    // Test SHA1
    var v1 = input.hash(2).hex();
	var v2 = "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12";
	if (v1 != v2)
		return false;

	v1 = new ByteArray("").hash(2).hex();
	v2 = "da39a3ee5e6b4b0d3255bfef95601890afd80709";
	if (v1 != v2)
		return false;

	return true;
}

function testHex()
{
	var binary = input.hash(2);

	var v1 = binary.hex();
	var v2 = new ByteArray(binary.hex(), 2).hex();
	if (v1 != v2)
		return false;

	v1 = new ByteArray(binary.hex(), 2).hex();
	if (v1 != binary.hex())
		return false;
	v1 = new ByteArray(binary.hex().toUpperCase(), 2).hex();
	if (v1 != binary.hex())
		return false;
	v1 = new ByteArray(binary.hex(1), 2).hex();
	if (v1 != binary.hex())
		return false;

	return true;
}

function testBase64()
{
	var binary = input.hash(2);

	v1 = binary.base64();

	var v1 = binary.base64();
	var v2 = new ByteArray(v1, 3);
	var v3 = v2.base64()
	if (v1 != v3)
		return false;
	if (v2.hex() != binary.hex())
		return false;

	return true;
}

function test(name, func)
{
	if (func())
		workspace += name + " OK\n";
	else
		workspace += name + " Error\n";
}

function runTests()
{
	workspace = "";
	test("hash", testHash);
	test("hex", testHex);
	test("base64", testBase64);
}

runTests();

