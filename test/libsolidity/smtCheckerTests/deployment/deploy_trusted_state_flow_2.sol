contract D {
	uint x;
	function f() public view returns (uint) { return x; }
}

contract C {
	D d;
	constructor() {
		d = new D();
		assert(d.f() == 0); // should hold
	}
	function g() public view {
		assert(d.f() == 0); // should hold
	}
}
// ====
// SMTEngine: all
// SMTExtCalls: trusted
