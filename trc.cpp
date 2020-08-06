#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

const std::string K_ms = "ms  ";

typedef unsigned long long ull;
typedef unsigned char uc;
typedef std::vector<ull> ulv;
typedef std::vector<std::string> stv;

struct my_numpunct : std::numpunct<char> {
    std::string do_grouping() const {return "\03";}
};

class InputParser{
    public:
        InputParser (int & argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        /// @author iain
        const std::string& getCmdOption(const std::string & option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string & option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

class DataSet {
    public:
        DataSet();
        DataSet(std::string idesc, std::string imn, std::function<void(std::string& a)> ifunc) {
            set_desc(idesc);
            set_method_name(imn);
            set_func(ifunc);
        }

        void reset_times() {
            times.clear();
        }

        void reset_inputs() {
            inputs.clear();
        }

        void reset_outputs() {
            outputs.clear();
        }

        /*         std::string get_times() { */
        /*             bool not_first = false; */
        /*             std::stringstream sst; */
        /*             for(auto a:times) { */
        /*                 if(not_first) { */
        /*                     sst << ','; */
        /*                 } */
        /*                 sst << "\"" << a << "\""; */
        /*                 not_first = true; */
        /*             } */
        /*             return sst.str(); */
        /*         } */

        std::string get_times() {
            return get_contents(times);
        }

        std::string get_inputs() {
            return get_contents(inputs);
        }

        std::string get_outputs() {
            return get_contents(outputs);
        }

        void append_time(ull& a) {
            times.emplace_back(a);
        }

        void append_input(std::string& s) {
            inputs.emplace_back(s);
        }

        void append_output(std::string& s) {
            outputs.emplace_back(s);
        }

        std::function<void(std::string & a)> get_func() {
            return func;
        }

        std::string get_desc() {
            return desc;
        }

        void set_desc(std::string idesc) {
            desc = idesc;
        }

        void set_func(std::function<void(std::string & a)> ifunc) {
            func = ifunc;
        }

        void set_method_name(std::string s) {
            method_name = s;
        }

        void set_t_start() {
            t_start = std::time(0);
        }

        void set_t_stop() {
            t_stop = std::time(0);
        }

        void set_t_max() {
            t_max = calc_max();
        }

        void set_t_mean() {
            t_mean = calc_mean();
        }

        void set_t_median() {
            t_median = calc_median();
        }

        void set_t_min() {
            t_min = calc_min();
        }

        std::string get_method_name() {
            return method_name;
        }

        ull get_t_max() {
            return t_max;
        }

        ull get_t_mean() {
            return t_mean;
        }

        ull get_t_median() {
            return t_median;
        }

        ull get_t_min() {
            return t_min;
        }

        ull get_t_start() {
            return t_start;
        }

        ull get_t_stop() {
            return t_stop;
        }

    private:
        std::function<void(std::string & a)> func;
        std::string desc;
        std::string method_name;
        stv inputs;
        stv outputs;
        ull t_max;
        ull t_mean;
        ull t_median;
        ull t_min;
        ull t_start;
        ull t_stop;
        ulv times;

        ull calc_max() {
            ull result = times[0];
            for(auto a:times) {
                result = result < a ? a : result;
            }
            return (result);
        }

        ull calc_mean() {
            ull sum = 0;
            for(auto n:times) {
                sum += n;
            }
            return sum / times.size();
        }

        ull calc_median() {
            std::sort(times.begin(), times.end());
            return times[times.size() >> 1];
        }

        ull calc_min() {
            ull result = times[0];
            for(auto a:times) {
                result = result > a ? a : result;
            }
            return (result);
        }

        template <class T>
            std::string get_contents(std::vector<T>& c) {
                bool not_first = false;
                std::stringstream sst;
                for(auto a:c) {
                    if(not_first) {
                        sst << ',';
                    }
                    sst << "\"" << a << "\"";
                    not_first = true;
                }
                return sst.str();
            }
};

typedef std::vector<DataSet> dsv;

std::string gft(bool do_d=false, bool do_t=true) {
    // get formatted time
    // Gets: bool (include date)
    //       bool (include time)
    // Returns: string
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::stringstream sst;

    if(do_d) {
        sst <<(now->tm_year + 1900) << '-'
            << std::setfill('0') << std::setw(2) << now->tm_mon + 1 << '-'
            << std::setfill('0') << std::setw(2) << now->tm_mday;
    }

    if(do_t) {
        if(do_d) {
            sst << 'T';
        }
        sst << std::setfill('0') << std::setw(2) << now->tm_hour << ':'
            << std::setfill('0') << std::setw(2) << now->tm_min  << ':'
            << std::setfill('0') << std::setw(2) << now->tm_sec;
    }

    return sst.str();
}

void progress_report(DataSet& a) {
    // Gets: ulv
    //       string
    // Returns: nothing
    std::locale loc (std::cout.getloc(),new my_numpunct);
    std::cout.imbue(loc);

    std::cout << '\n' << gft()
        << " " << a.get_method_name()
        << " " << a.get_desc()
        << std::flush;
}

void summarize(dsv& v) {
    // Gets: dsv
    // Returns: nothing
    std::cout << "\n\nStatistics:\n";
    for(auto a : v) {
        std::cout << "mean: " << a.get_t_mean() << K_ms
            << "min: " << a.get_t_min() << K_ms
            << "median: " << a.get_t_median() << K_ms
            << "max: " << a.get_t_max() << K_ms
            << " " << a.get_method_name()
            << std::endl;
    }
    std::cout << '\n' << std::endl;
}

void write_summary(dsv& v, std::string& ofn, ull& z, ull& n) {
    // Gets: dsv
    //       string
    //       ull
    //       ull
    //       returns: nothing
    if(ofn.size() > 0) {
        std::stringstream sst;
        sst << "{ \"file_name\" : \"" << ofn << "\", "
            << "\"time_stamp\" : \"" << gft(true) << "\", "
            << "\"string_size\" : \"" << z << "\", "
            << "\"sample_size\" : \"" << n << "\", "
            << "\"inputs\" : [" << v[0].get_inputs() << "], "
            << "\"methods\" : [";
        auto last = v.size() -  1;
        for(auto i = 0; i < v.size(); i++) {
            sst << "{\"description\":\"" << v[i].get_desc() << "\" , ";
            sst << "\"name\":\"" << v[i].get_method_name() << "\" , ";
            sst << "\"start\":\"" << v[i].get_t_start() << "\" , ";
            sst << "\"max\":\"" << v[i].get_t_max() << "\" , ";
            sst << "\"mean\":\"" << v[i].get_t_mean() << "\" , ";
            sst << "\"median\":\"" << v[i].get_t_median() << "\" , ";
            sst << "\"min\":\"" << v[i].get_t_min() << "\" , ";
            sst << "\"stop\":\"" << v[i].get_t_stop() << "\" , ";
            sst << "\"times\":[" << v[i].get_times();
            sst << "]}";
            if(i < last) {
                sst << ',';
            }
        }
        sst << "]}";
        std::ofstream of;
        of.open(ofn);
        of << sst.str();
        of.close();
    }
}

void m0(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i = toupper(*i);
    }
}

void m1(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -=  (*i >= 0x61 && *i <= 0x7a) << 5;
    }
}

void m2(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= *i >= 0x61 && *i <= 0x7a ? 0x20 : 0;
    }
}

void m3(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= (*i >= 0x61 && *i <= 0x7a) * 0x20;
    }
}

void m4(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= (*i >= 0x61) * (*i <= 0x7a) * 0x20;
    }
}

void m5(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= ((*i >= 0x61) * (*i <= 0x7a)) << 5;
    }
}

void m6(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= islower(*i) * 0x20;
    }
}

void m7(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= ((*i >= 0x61) * (*i <= 0x7a)) * 0x20;
    }
}

void m8(std::string& a) {
    for(auto i = a.begin(); i != a.end(); i++) {
        *i -= *i >= 0x61 ? *i <= 0x7a ? 0x20 : 0 : 0;
    }
}

bool verify(std::string& a) {
    for(auto c:a) {
        if(islower(c)) {
            return false;
        }
    }
    return true;
}

void test(stv& iv, DataSet& v, ull& ils) {
    v.reset_times();
    v.reset_times();
    v.reset_outputs();
    v.set_t_start();
    std::function<void(std::string& a)> func = v.get_func();
    bool first_time = true;
    for(auto i:iv) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string o(i);
        func(o);
        ull t = ((std::chrono::high_resolution_clock::now() - start).count());
        v.append_time(t);
        if(first_time) {
            first_time = false;
            if(!verify(o)) {
                std::cout << " Error from " << v.get_desc() << std::endl;
                std::cout << i.substr(0, 100) << '\n' << o.substr(0, 100) << std::endl;
                break;
            }
        }
        v.append_input(i);
        v.append_output(o);
    }
    v.set_t_max();
    v.set_t_mean();
    v.set_t_median();
    v.set_t_min();
    v.set_t_stop();
    progress_report(v);
}

ull prompt(std::string a) {
    ull n;
    std::cout << a;
    std::cin >> n;
    return n;
}

void init_string(std::string& s, ull& z) {
    const int range = 0x7e - 0x20;
    s.clear();
    for(ull c = 0; c < z; c++) {
        char r = rand() % range + 0x20;
        if(r == '"') {
            s += '\\';
        }
        s += r;
    }
}

void disp_help() {
    std::cout << "Command line options:\n"
        << "\t-h: Shows this screen\n"
        << "\t-p: Prompts for parameters\n"
        << "\t-z: Size of target string (default: 5,000,000)\n"
        << "\t-s: Sample size (default: 7)"
        << "\t-f: Output file name (default: \"\")"
        << std::endl;
}

void init_input_strings(stv& v, ull& n, ull& z) {
    v.clear();
    for(ull i = 0; i < n; i++) {
        std::string s("");
        init_string(s, z);
        v.emplace_back(s);
    }
    std::locale loc (std::cout.getloc(),new my_numpunct);
    std::cout.imbue(loc);
    std::cout << gft() << " " << v.size() << " Strings initialized." << std::flush;
}

bool is_numeric(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
            s.end(), [](uc c) { return !std::isdigit(c); }) == s.end();
}

void init_dsv(dsv& v, stv& s, ull n, ull z) {

    v.clear();
    init_input_strings(s, n, z);

    DataSet t("*i = toupper(*i)", "m0", m0);
    v.emplace_back(t);

    t.set_desc("*i -= (*i >= 0x61 && *i <= 0x7a) << 5");
    t.set_func(m1);
    t.set_method_name("m1");
    v.emplace_back(t);

    t.set_desc("*i -= *i >= 0x61 && *i <= 0x7a ? 0x20 : 0");
    t.set_func(m2);
    t.set_method_name("m2");
    v.emplace_back(t);

    t.set_desc("*i -= (*i >= 0x61 && *i <= 0x7a) * 0x20");
    t.set_func(m3);
    t.set_method_name("m3");
    v.emplace_back(t);

    t.set_desc("*i -= (*i >= 0x61) * (*i <= 0x7a) * 0x20");
    t.set_func(m4);
    t.set_method_name("m4");
    v.emplace_back(t);

    t.set_desc("*i -= ((*i >= 0x61) * (*i <= 0x7a)) << 5");
    t.set_func(m5);
    t.set_method_name("m5");
    v.emplace_back(t);

    t.set_desc("*i -= islower(*i) * 0x20");
    t.set_func(m6);
    t.set_method_name("m6");
    v.emplace_back(t);

    t.set_desc("*i -= ((*i >= 0x61) * (*i <= 0x7a)) * 0x20");
    t.set_func(m7);
    t.set_method_name("m7");
    v.emplace_back(t);

    t.set_desc("*i -= *i >= 0x61 ? *i <= 0x7a ? 0x20 : 0 : 0");
    t.set_func(m8);
    t.set_method_name("m8");
    v.emplace_back(t);
}

int main(int argc, char** argv) {
    InputParser input(argc, argv);
    if(input.cmdOptionExists("-h")) {
        disp_help();
        exit(0);
    }

    ull ssz = 5'000'000;
    ull sss = 7;
    std::string ofn("");

    if(input.cmdOptionExists("-p")) {
        ssz = prompt("String size?: ");
        sss = prompt("Sample space size?: ");
    } else {
        std::string sssz = input.getCmdOption("-z");
        if(is_numeric(sssz)) {
            ssz = stol(sssz);
        }
        std::string ssss = input.getCmdOption("-s");
        if(is_numeric(ssss)) {
            sss = stol(ssss) | 1; // must be odd for median
        }
        ofn = input.getCmdOption("-f");
    }

    std::locale loc (std::cout.getloc(),new my_numpunct);
    std::cout.imbue(loc);
    std::cout << gft()
        << " String size: " << ssz
        << " \tSample size: " << sss
        << " \tOutput file: " << ofn
        << std::endl;

    dsv dsc;
    stv isc;
    init_dsv(dsc, isc, sss, ssz);

    srand(time(NULL));
    for(int i = 0; i < dsc.size(); i++) {
        test(isc, dsc[i], sss);
    }
    summarize(dsc);
    write_summary(dsc, ofn, ssz, sss);

    return 0;
}
