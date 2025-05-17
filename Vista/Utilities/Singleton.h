#pragma once

namespace vista
{
	template <typename T>
	class Singleton
	{
	public:
		VISTA_NONCOPYABLE(Singleton)

		static Bool HasInstance() { return Instance != nullptr; }
		static T& Get()
		{
			if (Instance != nullptr)
			{
				return *Instance;
			}
			IsInitializing = true;
			Instance = new T();
			IsInitializing = false;
			return *Instance;
		};
		static void Reset()
		{
			VISTA_ASSERT_MSG(Instance != nullptr, "Attempting to destroy a singleton before it has been instantiated");
			IsDestroying = true;
			delete Instance;
			Instance = nullptr;
			IsDestroying = false;
		};

	protected:
		Singleton()
		{
			VISTA_ASSERT_MSG(IsInitializing, "Singleton not properly initialized. Did you forget to call Get()?");
		};

		~Singleton()
		{
			if (Instance != nullptr)
			{
				Reset();
			}
		};

	protected:
		static T* Instance;
		static Bool IsInitializing;
		static Bool IsDestroying;
	};

	template <typename T>
	T* Singleton<T>::Instance = nullptr;

	template <typename T>
	Bool Singleton<T>::IsInitializing = false;

	template <typename T>
	Bool Singleton<T>::IsDestroying = false;
}