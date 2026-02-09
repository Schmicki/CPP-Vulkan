#pragma once

namespace sge {

	/*Checked template class
	Stores value and bool, bool tells you if value was set. Can be used instead of a pointer.
	Check bool "hasVal" like you would "pointer == nullptr"*/
	template<typename T>
	class Checked {
	public:
		const bool hasVal;
		const T val;

		Checked() :hasVal(false), val(0) {}
		Checked(const T& _val) :hasVal(true), val(_val) {}

		explicit operator bool() const {
			return hasVal;
		}
	};
}