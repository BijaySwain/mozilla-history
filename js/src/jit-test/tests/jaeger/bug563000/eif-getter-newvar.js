// |jit-test| mjitalways;debug
setDebug(true);

this.__defineGetter__("someProperty", function () { evalInFrame(1, "var x = 'success'"); });
function caller(obj) {
  eval();
  obj.someProperty;
  return x;
}
assertEq(caller(this), "success");
