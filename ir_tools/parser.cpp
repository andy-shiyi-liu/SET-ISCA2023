#include <iostream>
#include <fstream>
#include "json/json.h"
#include <map>
#include <filesystem>

int main(int argc, char** argv){
	Json::Value IR;
	std::string filename = "IR.json";
	if(argc > 1){
		filename = std::string(argv[1]);
	}
	std::ifstream IRFile(filename);
	Json::Reader reader;
	reader.parse(IRFile, IR);
	IRFile.close();
	Json::Value max_workload;
	int max_buffer=0;
	Json::StyledWriter writer;
	int xlen = IR["xlen"].asUInt();
	int ylen = IR["ylen"].asUInt();
	for(int i=0;i<ylen*(xlen+2);++i){
		if(i%(xlen+2) && i%(xlen+2) != xlen+1){
			Json::Value &wlist = IR[std::to_string(i)];
			for(Json::Value &wl : wlist){
				int tot_buffer=0;
				for(Json::Value &buffer : wl["buffer"]){
					tot_buffer += buffer["block"].asInt();
				}
				if(tot_buffer > max_buffer){
					max_buffer = tot_buffer;
					max_workload = wl;
				}
				if(tot_buffer > 1024){
					std::cout << writer.write(wl);
				}
			}
		}
	}
	printf("max_buffer = %d\n",max_buffer);
	for(int i=0;i<ylen*(xlen+2);++i){
		if(i%(xlen+2) && i%(xlen+2) != xlen+1){
			Json::Value &wlist = IR[std::to_string(i)];
			int timestamp = 0;
			std::map<Json::Value, int> last, now, no;
			Json::Value input;
			input["top_batch_cut"] = IR["top_batch_cut"];
			int blockno = 0;
			for(Json::Value &wl : wlist){
				++timestamp;
				for(Json::Value &buffer : wl["buffer"]){
					bool newly_added = false;
					if(last.count(buffer)){
						now[buffer] = last[buffer];
						newly_added = false;
					}
					else if(!now.count(buffer)){
						now[buffer] = timestamp;
						no[buffer] = blockno++;
						newly_added = true;
					}
					else{
						Json::StyledWriter writer;
						std::cerr << writer.write(buffer);
					}
					buffer["no"] = no[buffer];
					if(buffer["type"] != "ofmap"){
						buffer["newly_added"] = newly_added;
					}
				}
				for(auto &buffer : last){
					if(!now.count(buffer.first)){
						Json::Value rect;
						rect["left"] = buffer.second;
						rect["right"] = timestamp - 1;
						rect["size"] = buffer.first["block"];
						rect["type"] = buffer.first["type"];
						rect["no"] = no[buffer.first];
						input["rects"].append(rect);
					}
				}
				last = now;
				now.clear();
			}
			++timestamp;
			for(auto &buffer : last){
				if(!now.count(buffer.first)){
					Json::Value rect;
					rect["left"] = buffer.second;
					rect["right"] = timestamp - 1;
					rect["size"] = buffer.first["block"];
					rect["type"] = buffer.first["type"];
					rect["no"] = no[buffer.first];
					input["rects"].append(rect);
				}
			}
			std::string dirname = "";
			if(argc > 2){
				dirname = std::string(argv[2])+"/";
			}
			if(dirname.size()){
				std::filesystem::create_directory(dirname+"wlist/");
			}
			std::ofstream wlfile(dirname+"wlist/"+std::to_string(i)+".json");
			Json::StyledWriter writer;
			auto str = writer.write(input);
			wlfile << str;
			wlfile.close();
		}
	}
	std::ofstream output(filename);
	output << writer.write(IR);
	output.close();
	return (xlen+2)*ylen;
}