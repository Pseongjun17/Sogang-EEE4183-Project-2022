
struct Flash_Data_st
{
    int valid_integer;

    int temp;
    bool is_Fernite;
    int hh;
    int mm;
    int ss;
    bool is_24H_system;
    bool is_scroll_vertical;
    bool is_char_rotated;
};


struct Flash_Data_st Flash_data;
extern struct Flash_Data_st Flash_data;

// Restore 
void restore_data_from_flash(void);
