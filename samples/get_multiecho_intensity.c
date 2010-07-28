/*!
  \example get_multiecho_intensity.c 距離・強度データ(マルチエコー)を取得する

  \author Satofumi KAMIMURA

  $Id$
*/


#include "urg_sensor.h"
#include "urg_utils.h"
#include <stdio.h>
#include <stdlib.h>


static void print_data(urg_t *urg, long data[],
                       unsigned short intensity[], int data_n, long time_stamp)
{
    // !!! とりあえずの csv 表示用
    int i;

    for (i = 0; i < data_n; ++i) {
        printf("%f, %ld, %ld, %ld, %d, %d, %d\n",
               urg_index2rad(urg, i),
               data[(3 * i) + 0], data[(3 * i) + 1], data[(3 * i) + 2],
               intensity[(3 * i) + 0], intensity[(3 * i) + 1], intensity[(3 * i) + 2]);
    }
    return;

#if 1
    (void)data_n;

    // 前方のデータのみを表示
    int front_index = 3 * urg_step2index(urg, 0);
    fprintf(stderr, "front_index: %d\n", front_index);
    printf("%ld, (%ld)\n", data[front_index], time_stamp);

#else
    int i;

    // 全てのデータを表示
    printf("# n = %d, time_stamp = %d\n", data_n, time_stamp);
    for (i = 0; i < data_n; ++i) {
        printf("%d, %ld\n", i, data[i]);
    }
#endif
}


int main(void)
{
    enum {
        CAPTURE_TIMES = 1,
    };
    urg_t urg;
    long *data = NULL;
    unsigned short *intensity = NULL;
    long time_stamp;
    //int ret;
    int n;
    int i;

    // 接続
#if 0
    if (urg_open(&urg, URG_SERIAL, "/dev/ttyACM0", 115200) < 0) {
#else
    if (urg_open(&urg, URG_SERIAL, "COM7", 115200) < 0) {
#endif
        printf("urg_open: %s\n", urg_error(&urg));
        return 1;
    }
    data = malloc((urg_max_index(&urg) + 1) * 3 * sizeof(data[0]));
    intensity = malloc((urg_max_index(&urg) + 1) * 3 * sizeof(intensity[0]));

    if (!data) {
        perror("urg_max_index()");
        return 1;
    }

    // データ取得
    fprintf(stderr, "measure_start: %d\n", urg_deg2index(&urg, -90));
#if 0
    ret = urg_set_scanning_parameter(&urg,
                                     urg_deg2step(&urg, -90),
                                     urg_deg2step(&urg, +90), 0);
    if (ret < 0) {
        // !!!
    }
#endif

    urg_start_measurement(&urg, URG_MULTIECHO_INTENSITY, CAPTURE_TIMES, 0);
    for (i = 0; i < CAPTURE_TIMES; ++i) {
        n = urg_get_distance_intensity(&urg, data, intensity, &time_stamp);
        fprintf(stderr, "[n = %d]\n", n);
        if (n <= 0) {
            printf("urg_distance: %s\n", urg_error(&urg));
            free(data);
            free(intensity);
            urg_close(&urg);
            return 1;
        }
        print_data(&urg, data, intensity, n, time_stamp);
    }

    // 切断
    free(data);
    free(intensity);
    urg_close(&urg);

    return 0;
}