#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RipRouter");

// 打断路由连接
void TearDownLink(Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
    nodeA->GetObject<Ipv4>()->SetDown(interfaceA);
    nodeB->GetObject<Ipv4>()->SetDown(interfaceB);
}

int main(int argc, char** argv)
{
    CommandLine cmd;
    cmd.Parse (argc, argv);

    Config::SetDefault("ns3::Rip::SplitHorizon", EnumValue(RipNg::POISON_REVERSE));

    // 节点与拓扑
    NS_LOG_INFO("Create nodes.");
    Ptr<Node> src = CreateObject<Node>();
    Names::Add("SrcNode", src);
    Ptr<Node> dst = CreateObject<Node>();
    Names::Add("DstNode", dst);
    Ptr<Node> a = CreateObject<Node>();
    Names::Add("RouterA", a);
    Ptr<Node> b = CreateObject<Node>();
    Names::Add("RouterB", b);
    Ptr<Node> c = CreateObject<Node>();
    Names::Add("RouterC", c);
    Ptr<Node> d = CreateObject<Node>();
    Names::Add("RouterD", d);
    Ptr<Node> e = CreateObject<Node>();
    Names::Add("RouterE", e);
    NodeContainer net1(src, a);
    NodeContainer net2(a, b);
    NodeContainer net3(b, e);
    NodeContainer net4(a, c);
    NodeContainer net5(c, d);
    NodeContainer net6(d, e);
    NodeContainer net7(e, dst);
    NodeContainer routers(a, b, c, d, e);
    NodeContainer nodes(src, dst);

    NS_LOG_INFO("Create channels.");
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(5000000));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    NetDeviceContainer ndc1 = csma.Install(net1);
    NetDeviceContainer ndc2 = csma.Install(net2);
    NetDeviceContainer ndc3 = csma.Install(net3);
    NetDeviceContainer ndc4 = csma.Install(net4);
    NetDeviceContainer ndc5 = csma.Install(net5);
    NetDeviceContainer ndc6 = csma.Install(net6);
    NetDeviceContainer ndc7 = csma.Install(net7);

    // 路由与IP
    NS_LOG_INFO("Create IPv4 and routing");
    RipHelper ripRouting;
    ripRouting.ExcludeInterface(a, 1);
    ripRouting.ExcludeInterface(e, 3);
    ripRouting.SetInterfaceMetric(b, 2, 10);
    ripRouting.SetInterfaceMetric(e, 1, 10);
    Ipv4ListRoutingHelper listRH;
    listRH.Add(ripRouting, 0);

    NS_LOG_INFO("Add IP Stack.");
    InternetStackHelper internet;
    internet.SetIpv6StackInstall(false);
    internet.SetRoutingHelper(listRH);
    internet.Install(routers);
    InternetStackHelper internetNodes;
    internetNodes.SetIpv6StackInstall(false);
    internetNodes.Install(nodes);

    // 定义IPv4地址
    NS_LOG_INFO("Assign IPv4 Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase(Ipv4Address("10.0.0.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic1 = ipv4.Assign(ndc1);
    ipv4.SetBase(Ipv4Address("10.0.1.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic2 = ipv4.Assign(ndc2);
    ipv4.SetBase(Ipv4Address("10.0.2.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic3 = ipv4.Assign(ndc3);
    ipv4.SetBase(Ipv4Address("10.0.3.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic4 = ipv4.Assign(ndc4);
    ipv4.SetBase(Ipv4Address("10.0.4.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic5 = ipv4.Assign(ndc5);
    ipv4.SetBase(Ipv4Address("10.0.5.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic6 = ipv4.Assign(ndc6);
    ipv4.SetBase(Ipv4Address("10.0.6.0"), Ipv4Mask("255.255.255.0"));
    Ipv4InterfaceContainer iic7 = ipv4.Assign(ndc7);

    Ptr<Ipv4StaticRouting> staticRouting;
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting>(src->GetObject<Ipv4>()->GetRoutingProtocol());
    staticRouting->SetDefaultRoute("10.0.0.2", 1);
    staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting>(dst->GetObject<Ipv4>()->GetRoutingProtocol());
    staticRouting->SetDefaultRoute("10.0.6.1", 1);

    // 打印路由表
    double time[24] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                       49.0, 50.0, 51.0, 52.0, 53.0, 78.0, 79.0, 80.0, 81.0, 82.0};  // 设置打印时间
    RipHelper routingHelper;
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);
    for(int i = 0; i< 20;i++)
    {
        routingHelper.PrintRoutingTableAt(Seconds(time[i]), a, routingStream);
        routingHelper.PrintRoutingTableAt(Seconds(time[i]), b, routingStream);
        routingHelper.PrintRoutingTableAt(Seconds(time[i]), c, routingStream);
        routingHelper.PrintRoutingTableAt(Seconds(time[i]), d, routingStream);
        routingHelper.PrintRoutingTableAt(Seconds(time[i]), e, routingStream);
    }

    // 应用，载荷1024字节，一秒一个包
    NS_LOG_INFO("Create Applications.");
    V4PingHelper ping("10.0.6.2");
    ping.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    ping.SetAttribute("Size", UintegerValue(1024));
    ping.SetAttribute("Verbose", BooleanValue(true));
    ApplicationContainer apps = ping.Install(src);
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(90.0));

    // 跟踪
    NS_LOG_INFO("Configure Tracing.");
    AsciiTraceHelper ascii;
    csma.EnableAsciiAll(ascii.CreateFileStream("RIP.tr"));
    csma.EnablePcapAll("RIP", true);

    // 断开DE之间的连接
    Simulator::Schedule(Seconds(50), &TearDownLink, d, e, 2, 2);

    // 启动
    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(100.0));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
}