#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

// L1: forgot delete
void leak_forgot_delete() {
    int* p = new int(42);
    std::cout << "L1: " << *p << " at " << p << "\n";
    // forgot: delete p;
}

// L2: lost pointer before delete
void leak_lost_pointer(){
    int* p = new int(42);
    p = new int(42);    // first allocation is now unreachable
    delete p;           // only fires the second
}

// L3: early return / exeption bypasses cleanup
void leak_early_return(bool doThrow){
    int* p = new int(42);
    if(doThrow) throw std::runtime_error("boom");
    // if we returned early here we'd leak; we throw instead
    delete p;
}

// L4: new[] with scalar delete (undefined behavior)
void leak_mismatched_delete(){
    int* arr = new int[100];
    delete arr;         // WRONG: should be delete[]
}

// L5: container of raw pointers cleared wihtout deleting
void leak_container_of_raw(){
    struct Foo {int x;};
    std::vector<Foo*> v;
    v.push_back(new Foo{1});
    v.push_back(new Foo{2});
    v.clear();          // Foo objects leaked
}

// L6: assigmnet operator overwrites old allocation
struct Widget{
    int* data = nullptr;
    Widget() : data(new int(0)){}
    Widget& operator=(const Widget& other){
        data = new int(*other.data);    // old data leak
        return *this;
    }
};

void leak_self_assignment_confusion(){
    Widget a; *a.data = 1;
    Widget b; *b.data = 2;
    a = b;              // a's orginal int leaks
}

// ---------------------------------------------------------------------------
// DANGLING POINTER EXAMPLES
// ---------------------------------------------------------------------------

// D1: use after delete
void dangling_use_after_delete(){
    int* p = new int(42);
    delete p;
    std::cout << "D1: " << *p << "\n";  // UAF read
    *p = 10;                            // UAF write
}

// D2: pointer to local that went out of scope
int* dangling_local(){
    int x = 42;
    return &x;      // x dies on return
}
void dangling_local_use(){
    int* p = dangling_local();
    std::cout << "D2: "<< *p << "\n";   // UAF
}

// D3: reference to local return
const std::string& dangling_ref(){
    std::string s ="hello";
    return s;           // reference to destroyed local
}
void dangling_ref_use(){
    const std::string& r = dangling_ref();
    std::cout << "D3: " << r << "\n";   // UAF
}

// D4: container reallocation invaidates pointer / iterator
void dangling_vector_realoc(){
    std::vector<int> v {1,2,3};
    int* p = &v[0];
    v.push_back(4);     // may realocate; p is now dangling
    std::cout << "D4: " << *p << "\n";  // UAF
}

// D5: double delete
void dangling_double_delete(){
    int* p = new int(5);
    int* q = p;
    delete p;
    delete q;       // double free
}

// D6: using a raw pointer after its unique_ptr is reset
void dangling_raw_after_reset(){
    std::unique_ptr<int> a = std::make_unique<int>(1);
    int* raw = a.get();
    a.reset();          // frees the int
    std::cout << "D6: " << *raw << "\n"; //UAF
}

// D7: pointer to element whose container is destroyed
void dangling_container_destroyed(){
    int* p;
    {
        std::vector<int> v = {1,2,3,4,5};
        p = &v[2];
    }               // v destroyed; p dangling
    std::cout << "D7: " << *p << "\n";  // UAF
}

// ---------------------------------------------------------------------------
// Dispatch
// ---------------------------------------------------------------------------

int main(int argc, char** argv){
    if (argc < 2){
        std::cerr << "usage: " <<argv[0] << " <case\n"
                  << "cases: L1 L2 L3 L4 L5 L6 D1 D2 D3 D4 D5 D6 D7 all\n";
        return 1;
    }
    std::string c = argv[1];

    try{
        if  (c== "L1") leak_forgot_delete();
        else if (c== "L2") leak_lost_pointer();
        else if (c== "l3") leak_early_return(true);
        else if (c== "l4") leak_mismatched_delete();
        else if (c== "L5") leak_container_of_raw();
        else if (c== "L6") leak_self_assignment_confusion();

        else if (c== "D1") dangling_use_after_delete();
        else if (c== "D2") dangling_local_use();
        else if (c== "D3") dangling_ref_use();
        else if (c== "D4") dangling_vector_realoc();
        else if (c== "D5") dangling_double_delete();
        else if (c== "D6") dangling_raw_after_reset();
        else if (c== "D7") dangling_container_destroyed();

        else if (c=="all"){
            // Runs leaks first(they don't crash under Asan, just report at exit),
            // then the dangling - the process will abort on the first UAF
            leak_forgot_delete();
            leak_lost_pointer();
            leak_mismatched_delete();
            leak_container_of_raw();
            leak_self_assignment_confusion();
            // :3 throws; skip it in "all" so we reach the danglers
            std::cout << "\n--- now the dangling cases ---\n";
            dangling_use_after_delete(); // ASam will abort here
        }
        else{
            std::cerr << "unknown case: " << c << "\n";
            return 1;
        }
    } catch(const std::exception& e){
        std::cerr << "exception: " << e.what() << "\n"; 
    }
    std::cout << "case" << c << " returned\n";
    return 0;
}