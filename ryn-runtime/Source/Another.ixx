export module hello;
import std;

export namespace hello {
	void say() {
		std::cout << "Hello, Module!" << std::endl;
	}
}