#include <entt/entt.hpp>
#include <iostream>

using namespace std;

int f(int i) 
{
    cout << "called f" << endl;
    return i; 
}

struct my_struct {
    int f(const int &i) const 
    {
        cout << "called my_struct" << endl;
        return i; 
    }
};

struct info
{
    info(int _i) : i(_i) {}
    int i;
};

info g_i(15);

int func(info& i)
{
    cout << "info : " << i.i << endl;
    return i.i;
}

int main()
{
    {
        entt::delegate<int(int)> delegate{};

        // bind a free function to the delegate
        delegate.connect<&f>();

        // bind a member function to the delegate
        //my_struct instance;
        //delegate.connect<&my_struct::f>(instance);

        int result = delegate(42);
        cout << "Result : " << result << endl;
    }
    {
        entt::delegate<int(info&)> delegate_struct{};
        delegate_struct.connect<&func>();

        info i(10);
        delegate_struct(i);
        delegate_struct(g_i);
    }

    return 0;
}