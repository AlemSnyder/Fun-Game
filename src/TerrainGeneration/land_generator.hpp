#include <stdint.h>
#include "json/json.h"
#include <map>
#include <math.h>
#include <string>
#include <iostream>
#include <string>

#include "tile_stamp.hpp"
#include "material.hpp"

#ifndef __LAND_GENERATOR_HPP__
#define __LAND_GENERATOR_HPP__

class Land_Generator{
public:
    Land_Generator( const std::map<int, const Material> *materials_, Json::Value data_){
        materials = materials_;
        data = data_;
        current_region = 0;
        current_sub_region = 0;
    };
    Land_Generator(){
        //materials = Material();
        //data = ;
        current_region = 0;
        current_sub_region = 0;
    };


    bool empty()const{
        return (current_region >= data.size());
    }
    Tile_Stamp get_this_stamp(){
        Tile_Stamp out;
        out.mat = &(*materials).at((int)data[current_region]["Material_id"].asInt());
        out.color_id = data[current_region]["Color_id"].asInt();
        for (Json::Value::ArrayIndex i = 0; i < data[current_region]["Can_Stamp"].size(); i++ ){
            int E = data[current_region]["Can_Stamp"][i]["E"].asInt();
            if (data[current_region]["Can_Stamp"][i]["C"].isInt()){
                int C = data[current_region]["Can_Stamp"][i]["C"].asInt();
                out.elements_canstamp.insert(std::make_pair(E, C));
            }
            else if (data[current_region]["Can_Stamp"][i]["C"].asBool()){
                for (unsigned int C = 0; C < (*materials).at(E).color.size(); C++){
                    out.elements_canstamp.insert(std::make_pair(E, C));
                }
            }
        }
        std::string type = data[current_region]["Type"].asCString();

        if (type == "Positions" ){
            From_Positions(current_region, current_sub_region, out);
        } else if (type == "Radius"){
            From_Radius(current_region, current_sub_region, out);
        } else if (type == "Grid"){
            From_Grid(current_region, current_sub_region, out);
        }
        return out;
    }
    
    void operator++(){
        current_sub_region++;
        if (current_sub_region == get_num_stamps(data[current_region])){
            current_region++;
            current_sub_region=0;
        }
    }
    void reset(){
        current_region = 0;
        current_sub_region = 0;
    };

private:
    unsigned int current_region;
    unsigned int current_sub_region;

    Json::Value data; //this should be a structure
    const std::map<int, const Material> *materials;

    unsigned int static get_num_stamps(Json::Value biome){
        if (biome["Type"].asCString()== std::string( "Positions")){
            return biome["Positions"].size();
        } else if (biome["Type"].asCString()==std::string("Grid")){
            return pow(biome[biome["Type"].asCString()]["number"].asInt(), 2);
        } else{
            return biome[biome["Type"].asCString()]["number"].asInt();
        }
    }

    std::array<int, 6> get_volume(int center[2][2], int Sxy, int Sz, int Dxy, int Dz ){

        int center_x = rand() % ( center[1][0]-center[0][0] + 1) + center[0][0];
        int center_y = rand() % ( center[1][1]-center[0][1] + 1) + center[0][1];
        int size_x = rand() % ( 2 * Dxy + 1) + Sxy - Dxy;
        int size_y = rand() % ( 2 * Dxy + 1) + Sxy - Dxy;
        int x_min = center_x - size_x/2;
        int x_max = center_x + size_x/2;
        int y_min = center_y - size_y/2;
        int y_max = center_y + size_y/2;

        if (! (size_x % 2)){
            if ( rand() % 2 - 1 ){
                x_min--;
            } else {
                x_max--;
            }
        }
        if (! (size_y % 2)){
            if ( rand() % 2  - 1 ){
                y_min--;
            } else {
                y_max--;
            }
        }

        int z_max = rand() % ( Dz + 1) + Sz - Dz/2;
        return {x_min, y_min, 0, x_max, y_max, z_max};
    }

    void From_Radius(int cr, int csr, Tile_Stamp & ts){
        int radius = data[cr]["Radius"]["radius"].asInt();
        double distance = (double) (8 * radius) / data[cr]["Radius"]["number"].asInt() * csr;
        int side = (int) distance / 2 / radius;
        int x_center, y_center;

        if (side == 0){
            x_center = -radius;
            y_center = -radius+std::fmod(distance, 2*radius);
        }else if (side == 1){
            x_center = -radius+std::fmod(distance, 2*radius);
            y_center = radius;
        }else if (side == 2){
            x_center = radius;
            y_center = radius-std::fmod(distance, 2*radius);
        }else if (side == 3){
            x_center = radius-std::fmod(distance, 2*radius);
            y_center = -radius;
        } else{
            throw std::invalid_argument("Something went horribly wrong");
        }

        int DC = data[cr]["Radius"]["DC"].asInt();
        int center [2][2] = {{x_center-DC, y_center-DC}, 
                             {x_center+DC, y_center+DC} };

        std::array<int, 6> volume = get_volume(center, data[cr]["Size"].asInt(), data[cr]["Hight"].asInt(), data[cr]["DS"].asInt(), data[cr]["DH"].asInt());

        ts.x_start = volume[0];
        ts.y_start = volume[1];
        ts.z_start = volume[2];
        ts.x_end = volume[3];
        ts.y_end = volume[4];
        ts.z_end = volume[5];
    }
    void From_Grid(int cr, int csr, Tile_Stamp &ts){
        int x_center = (1 + 2 * (csr % data[cr]["Grid"]["number"].asInt())) * ( data[cr]["Grid"]["radius"].asInt() / data[cr]["Grid"]["number"].asInt()) - data[cr]["Grid"]["radius"].asInt();
        int y_center = (1 + 2 * (csr / data[cr]["Grid"]["number"].asInt())) * ( data[cr]["Grid"]["radius"].asInt() / data[cr]["Grid"]["number"].asInt()) - data[cr]["Grid"]["radius"].asInt();
        int DC = data[cr]["Grid"]["DC"].asInt();

        int center [2][2] = {{x_center-DC, y_center-DC}, 
                             {x_center+DC, y_center+DC} };

        std::array<int, 6> volume = get_volume(center, data[cr]["Size"].asInt(), data[cr]["Hight"].asInt(), data[cr]["DS"].asInt(), data[cr]["DH"].asInt());

        ts.x_start = volume[0];
        ts.y_start = volume[1];
        ts.z_start = volume[2];
        ts.x_end = volume[3];
        ts.y_end = volume[4];
        ts.z_end = volume[5];

    }
    void From_Positions(int cr, int csr, Tile_Stamp &ts){
        Json::Value xy_positions = data[cr]["Positions"][csr];
        int center [2][2] = {{xy_positions[0].asInt(), xy_positions[1].asInt()}, 
                             {xy_positions[0].asInt(), xy_positions[1].asInt()} };

        std::array<int, 6> volume = get_volume(center, data[cr]["Size"].asInt(), data[cr]["Hight"].asInt(), data[cr]["DS"].asInt(), data[cr]["DH"].asInt());

        ts.x_start = volume[0];
        ts.y_start = volume[1];
        ts.z_start = volume[2];
        ts.x_end = volume[3];
        ts.y_end = volume[4];
        ts.z_end = volume[5];
    }
};

#endif