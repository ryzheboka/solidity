type MyInt is int;
function test() pure {
    function (MyInt) returns (int) f = MyInt.unwrap;
    function (int) returns (MyInt) g = MyInt.wrap;
}
// ----
// TypeError 9574: (46-93): Type BUILTIN:19 (MyInt) pure returns (int256) is not implicitly convertible to expected type function (MyInt) returns (int256). Special functions can not be converted to function types.
// TypeError 9574: (99-144): Type BUILTIN:18 (int256) pure returns (MyInt) is not implicitly convertible to expected type function (int256) returns (MyInt). Special functions can not be converted to function types.
