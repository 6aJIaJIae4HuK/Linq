#pragma once

#include <memory>

template<typename ParentView>
class ReversedView {
private:
	std::shared_ptr<ParentView> Parent;
public:
	ReversedView(std::shared_ptr<ParentView> parent)
		: Parent(parent)
	{}

	auto begin() const {
		return Parent->rbegin();
	}

	auto end() const {
		return Parent->rend();
	}

	auto rbegin() const {
		return Parent->begin();
	}

	auto rend() const {
		return Parent->end();
	}
};
