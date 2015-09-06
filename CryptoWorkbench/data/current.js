//workspace = new Int8Array(File.read("test.cwb"));

//workspace = hello();

//workspace = new Int8Array(Tools.hash(File.read("test.cwb"), 3));

function test()
{
	var ba = new ByteArray("test");
	//workspace = ba.buffer;
	workspace = ba.hex();
}

test();


//load("test.cwb");

//output = Bench.replaceLetters();
//output = Tools.replaceLetters();
//
//var a = new ByteArray();