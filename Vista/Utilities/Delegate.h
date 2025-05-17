#pragma once
#include <functional>
#include <vector>
#include <concepts>

namespace vista
{
#define DECLARE_DELEGATE(name, ...) \
	using name = Delegate<void(__VA_ARGS__)>;

#define DECLARE_DELEGATE_RETVAL(name, retval, ...) \
	using name = Delegate<retval(__VA_ARGS__)>;

#define DECLARE_MULTICAST_DELEGATE(name, ...) \
	using name = MultiCastDelegate<__VA_ARGS__>; \

#define DECLARE_EVENT(name, owner, ...) \
	class name : public MultiCastDelegate<__VA_ARGS__> \
	{ \
	private: \
		friend class owner; \
		using MultiCastDelegate<__VA_ARGS__>::Broadcast; \
		using MultiCastDelegate<__VA_ARGS__>::RemoveAll; \
		using MultiCastDelegate<__VA_ARGS__>::Remove; \
	};

	template<typename...>
	class Delegate;

	template<typename R, typename... Args>
	class Delegate<R(Args...)>
	{
		using DelegateType = std::function<R(Args...)>;
	public:
		Delegate() = default;
		VISTA_DEFAULT_COPYABLE_MOVABLE(Delegate)
		~Delegate() = default;

		template<typename F> requires std::is_constructible_v<DelegateType, F>
		void BindLambda(F&& lambda)
		{
			callback = lambda;
		}

		void BindStatic(R(*pf)(Args...))
		{
			callback = pf;
		}

		template<typename T>
		void BindMember(R(T::*mem_pfn)(Args...), T& instance)
		{
			callback = [&instance, mem_pfn](Args&&... args) mutable -> R {return (instance.*mem_pfn)(std::forward<Args>(args)...); };
		}

		void Unbind()
		{
			callback = nullptr;
		}

		R Execute(Args... args) const
		{
			return callback(args...);
		}
		R ExecuteIfBound(Args... args) const
		{
			return IsBound() ? callback(args...) : R();
		}

		R operator()(Args... args) const
		{
			return callback(args...);
		}

		Bool IsBound() const { return callback != nullptr; }

		template<typename F> requires std::is_constructible_v<DelegateType, F>
		static Delegate CreateLambda(F&& lambda)
		{
			Delegate delegate;
			delegate.BindLambda(std::forward<F>(lambda));
			return delegate;
		}
		static Delegate CreateStatic(R(*pf)(Args...))
		{
			Delegate delegate;
			delegate.BindStatic(pf);
			return delegate;
		}
		template<typename T>
		static Delegate CreateMember(R(T::* mem_pfn)(Args...), T& instance)
		{
			Delegate delegate;
			delegate.BindMember(mem_pfn, instance);
			return delegate;
		}

	private:
		DelegateType callback = nullptr;
	};

	class DelegateHandle
	{
	public:
		DelegateHandle() : id(INVALID_ID) {}
		explicit DelegateHandle(int) : id(GenerateID()) {}
		VISTA_DEFAULT_COPYABLE(DelegateHandle)
		DelegateHandle(DelegateHandle&& that) noexcept : id(that.id)
		{
			that.Reset();
		}
		DelegateHandle& operator=(DelegateHandle&& that) noexcept
		{
			id = that.id;
			that.Reset();
			return *this;
		}
		~DelegateHandle() noexcept = default;

		operator Bool() const
		{
			return IsValid();
		}

		Bool operator==(DelegateHandle const& that) const
		{
			return id == that.id;
		}

		Bool operator<(DelegateHandle const& that) const
		{
			return id < that.id;
		}

		Bool IsValid() const
		{
			return id != INVALID_ID;
		}

		void Reset()
		{
			id = INVALID_ID;
		}

	private:
		Uint64 id;

	private:

		inline static constexpr Uint64 INVALID_ID = Uint64(-1);
		static Uint64 GenerateID()
		{
			static Uint64 current_id = 0;
			return current_id++;
		}
	};

	template<typename... Args>
	class MultiCastDelegate
	{
		using DelegateType = Delegate<void(Args...)>;
		using HandleDelegatePair = std::pair<DelegateHandle, DelegateType>;

	public:
		MultiCastDelegate() = default;
		VISTA_DEFAULT_COPYABLE_MOVABLE(MultiCastDelegate)
		~MultiCastDelegate() = default;

		VISTA_MAYBE_UNUSED DelegateHandle Add(DelegateType&& handler)
		{
			delegateArray.emplace_back(DelegateHandle(0), std::forward<DelegateType>(handler));
			return delegateArray.back().first;
		}
		VISTA_MAYBE_UNUSED DelegateHandle Add(DelegateType const& handler)
		{
			delegateArray.emplace_back(DelegateHandle(0), handler);
			return delegateArray.back().first;
		}

		template<typename F> 
		VISTA_MAYBE_UNUSED DelegateHandle AddLambda(F&& lambda)
		{
			return Add(DelegateType::CreateLambda(std::forward<F>(lambda)));
		}

		template<typename R>
		VISTA_MAYBE_UNUSED DelegateHandle AddStatic(R(*pf)(Args...))
		{
			return Add(DelegateType::CreateStatic(pf));
		}

		template<typename T>
		VISTA_MAYBE_UNUSED DelegateHandle AddMember(void(T::* mem_pfn)(Args...), T& instance)
		{
			return Add(DelegateType::CreateMember(mem_pfn, instance));
		}

		VISTA_MAYBE_UNUSED Bool Remove(DelegateHandle& handle)
		{
			if (handle.IsValid())
			{
				for (Uint64 i = 0; i < delegateArray.size(); ++i)
				{
					if (delegateArray[i].first == handle)
					{
						std::swap(delegateArray[i], delegateArray.back());
						delegateArray.pop_back();
						handle.Reset();
						return true;
					}
				}
			}
			return false;
		}

		void RemoveAll()
		{
			delegateArray.clear();
		}

		void Broadcast(Args... args)
		{
			for (Uint64 i = 0; i < delegateArray.size(); ++i)
			{
				if (delegateArray[i].first.IsValid()) delegateArray[i].second.ExecuteIfBound(std::forward<Args>(args)...);
			}
		}

		Bool IsHandleBound(DelegateHandle const& handle) const
		{
			if (handle.IsValid())
			{
				for (Uint64 i = 0; i < delegateArray.size(); ++i)
				{
					if (delegateArray[i].Handle == handle) return true;
				}
			}
			return false;
		}

	private:
		std::vector<HandleDelegatePair> delegateArray;
	};
}