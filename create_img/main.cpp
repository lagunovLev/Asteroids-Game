#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <time.h>
#include <uchar.h>
#include <vector>
using namespace std;

struct __attribute__ ((packed)) MBR_Partition
{
    uint8_t boot_indicator;
    uint8_t starting_chs[3];
    uint8_t os_type;
    uint8_t ending_chs[3];
    uint32_t starting_lba;
    uint32_t size_lbas;
};

struct __attribute__ ((packed)) MBR
{
    uint8_t boot_code[446];
    MBR_Partition partition[4];
    uint16_t boot_signature;
};

string dir;
string output_file_name = "os.img";
uint32_t sectors_number_position;

uint32_t round_to_sectors(uint64_t bytes)
{
    return bytes + (512 - bytes % 512);
}

void write_buffer(vector<unsigned char>& buffer, ofstream& stream, char fill)
{
    for (int i = 0; i < round_to_sectors(buffer.size()); i++)
    {
        if (i < buffer.size())
            stream.put(buffer[i]);
        else
            stream.put(fill);
    }
}

int main(int argc, char* argv[])
{ 
    string dir = argv[1];

    ifstream input(dir + "/bootloader/zero_sector.bin", ios::binary);
    vector<unsigned char> buffer(istreambuf_iterator<char>(input), {});
    input.close();

    input.open(dir + "kernel.bin", ios::binary);
    vector<unsigned char> buffer_ex(istreambuf_iterator<char>(input), {});
    input.close();

    if (buffer.size() > 446)
    {   
        cout << "zero_sector.bin is larger than 440 bytes" << endl;
        return 1; 
    }

    sectors_number_position = (buffer[3] << 8) + buffer[2] - 0x7c00;
    
    buffer[sectors_number_position] = round_to_sectors(buffer_ex.size()) / 512; 
    cout << "Kernel size: " << round_to_sectors(buffer_ex.size()) / 512 << endl;
    
    MBR mbr = {
        .boot_code = { 0 },
        .partition = { 0 }, 
        .boot_signature = 0xAA55,
    };

    for (int i = 0; i < 446; i++)
    {
        if (i < buffer.size())
            mbr.boot_code[i] = buffer[i];
        else
            mbr.boot_code[i] = 0;
    }

    //mbr.partition[0] = {
    //    .boot_indicator = 0x80,
    //    .starting_chs = { 0x00, 0x00, 0x02 },
    //    .os_type = 0xEE,
    //    .ending_chs = { 0b11111110, 0b11111011, 0b11111110 },
    //    .starting_lba = 0x00000001,
    //    .size_lbas = 0x00000400,
    //};

    ofstream output(dir + output_file_name, ios::binary | ios::out);
    output.write((char*)&mbr, sizeof(mbr));
    
    write_buffer(buffer_ex, output, 0);

    for (int i = 0; i < 0; i++)
    {
        output.put(0);
    }

    output.close();

    return 0;
}