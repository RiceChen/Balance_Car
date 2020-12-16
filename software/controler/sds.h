#ifndef CONTROLER_SDS_H_
#define CONTROLER_SDS_H_

struct sds_msg
{
    float ch1_data;
    float ch2_data;
    float ch3_data;
    float ch4_data;
};

void sds_output_data(struct sds_msg *msg);

#endif /* CONTROLER_SDS_H_ */
