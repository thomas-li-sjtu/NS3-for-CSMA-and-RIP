import os
import pandas as pd
import glob
import matplotlib.pyplot as plt
import numpy as np


def combine(path='pcap/*.csv', name="result.csv"):
    csv_list = glob.glob(path)  # 查看同文件夹下的csv文件数
    print(u'共发现%s个CSV文件' % len(csv_list))
    print(u'正在处理............')
    for i in csv_list:  # 循环读取同文件夹下的csv文件
        fr = open(i, 'rb').read()
        with open(name, 'ab') as f:  # 将结果保存为result.csv
            f.write(fr)
    print(u'合并完毕！')


def sort_data(path="result.csv"):
    data = pd.read_csv(path)
    data = data.sort_values(by='Time')
    data.to_csv("sorted_result.csv", index=None)


def throughout(path):
    result = []
    with open(path, "r", encoding="utf-8") as file:
        for i in file:
            i = i[:-1].split("\t")
            if i[-1] != 0:
                result.append(float(i[-1]))

    return round(sum(result)/len(result), 3)


def calc_g(path):
    g_result = []
    with open(path, "r", encoding="utf-8") as file:
        for i in file:
            i = i[:-1].split(" ")
            if i[0] == "+" and "ns3::UdpHeader" in i:
                if int(float(i[1])) > len(g_result):
                    g_result.append(0)
                g_result[int(float(i[1]))-1] += 1

    if len(g_result) > 9:
        g_result.remove(min(g_result))
    # print(g_result)
    return round(sum(g_result)/9, 3), round(sum(g_result) / len(g_result), 3)


if __name__ == '__main__':
    # combine("pcap/todif/source/*.csv", name="source_all.csv")
    # sort_data("source_all.csv")
    throughout_paths = os.listdir("throughout")
    s_dict = {}
    g_dict = {}
    for i in sorted(throughout_paths):
        s_dict[i.split("thoughout")[-1]] = throughout(os.path.join("throughout", i))
    print(s_dict)

    trace_paths = os.listdir("pcap")
    for i in sorted(trace_paths):
        g_dict[i] = calc_g(os.path.join("pcap", i, "csma-one-subnet.tr"))
    print(g_dict)

    plot_dict = {g_dict[key][0]: s_dict[key] for key in s_dict.keys()}
    plot_dict = sorted(plot_dict.items())
    print(plot_dict)
    x, y = [x[0] for x in plot_dict], [y[1] for y in plot_dict]
    # plt.scatter(x, y, alpha=0.6, edgecolors='white')
    # plt.title('S-G graph')
    # plt.xlabel('attempts per second')
    # plt.ylabel('throughout')
    # plt.legend(loc='best')
    # plt.grid(True)
    # plt.show()

    z1 = np.polyfit(x, y, 5)  # 用4次多项式拟合
    p1 = np.poly1d(z1)
    print(p1)  # 打印拟合多项式
    yvals = p1(x)
    plot1 = plt.plot(x, y, '*', label='original values')
    plot2 = plt.plot(x, yvals, 'r', label='polyfit values')
    plt.title('S-G graph')
    plt.xlabel('attempts per second')
    plt.ylabel('throughout')
    # plt.legend(loc=4)  # 指定legend的位置,读者可以自己help它的用法
    plt.grid(True)
    plt.show()
# package_time = {i: [0.010928*i, 0] for i in range(10000) if 0.010928*i < 15}
# data = pd.read_csv("sorted_result.csv")
# for i in data.values:
#     i = list(i)
#     try:
#         if i[4] == "UDP":
#             index = float(i[1]) // 0.010928
#             package_time[index] = [package_time[index][0], package_time[index][1]+1]
#     except:
#         continue
# for key, value in package_time.values():
#     print(key, value)

