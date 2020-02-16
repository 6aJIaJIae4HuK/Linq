#pragma once

#include "linq.h"

#include <functional>

template<typename T, class = typename std::enable_if_t<std::is_integral_v<T>>>
class RangeCollection : public ICollection<T> {
private:
	class IteratorConsumer : public IIteratorConsumer<T> {
	private:
		using CheckerType = std::function<bool(const T&, const T&)>;
		T Start;
		T Finish;
		T Step;
		T Cur;
		CheckerType Checker;

		static inline auto ForAscending = [](const T& finish, const T& cur) {
			return cur < finish;
		};

		static inline auto ForDescending = [](const T& finish, const T& cur) {
			return finish < cur;
		};

		static CheckerType GetChecker(const T& start, const T& finish) {
			if (start < finish)
				return ForAscending;
			return ForDescending;
		}

	public:
		IteratorConsumer(T start, T finish, T step)
			: Start(start)
			, Finish(finish)
			, Step(step)
			, Checker(GetChecker(Start, Finish))
		{}

		bool IterateToFirst() override {
			Cur = Start;
			return Checker(Finish, Cur);
		}

		bool IterateToNext() override {
			Cur += Step;
			return Checker(Finish, Cur);
		}

		bool IterateToLast() override {
			if (Start == Finish)
				return false;
			Cur = Start + (Finish - Start) / Step;
			if (Cur == Finish) {
				Cur -= Step;
			}
			return Checker(Finish, Cur);
		}

		bool IterateToPrev() override {
			if (Cur == Start)
				return false;
			Cur -= Step;
			return true;
		}

		T GetCurrent() const override {
			return Cur;
		}
	};
	mutable IteratorConsumer Consumer;
public:
	RangeCollection(T start, T finish, T step)
		: Consumer(start, finish, step)
	{}

	IIteratorConsumer<T>* GetConsumer() const override {
		return &Consumer;
	}
};

template<typename T, class>
inline Collection<T> Range(T start, T finish, T step) {
	return Collection<T>(std::make_shared<RangeCollection<T>>(start, finish, step));
}

template<typename T, class>
inline Collection<T> Range(T finish) {
	return Range(T{}, finish);
}
