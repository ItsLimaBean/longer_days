#pragma once
namespace longer_days
{
	template<typename T>
	class singleton
	{
	public:
		static T& get()
		{
			static T instance{ tkn{} };
			return instance;
		}

		singleton(const singleton&) = delete;
		singleton& operator= (const singleton) = delete;

	protected:
		struct tkn {};
		singleton() {};
	};
}
