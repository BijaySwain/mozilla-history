// Basic getOffsetLine test, using Error.lineNumber as the gold standard.

var g = newGlobal('new-compartment');
var dbg = Debug(g);
var hits;
dbg.hooks = {
    debuggerHandler: function (frame) {
	var knownLine = frame.eval("line").return;
	assertEq(frame.script.getOffsetLine(frame.offset), knownLine);
	hits++;
    }
};

hits = 0;
g.eval("var line = new Error().lineNumber; debugger;");
assertEq(hits, 1);

hits = 0;
g.eval("var s = 2 + 2;\n" +
       "s += 2;\n" +
       "line = new Error().lineNumber; debugger;\n" +
       "s += 2;\n" +
       "s += 2;\n" +
       "line = new Error().lineNumber; debugger;\n" +
       "s += 2;\n" +
       "assertEq(s, 12);\n");
assertEq(hits, 2);
