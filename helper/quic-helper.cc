#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "quic-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("QuicHelper");

QuicHelper::QuicHelper ()
{
  SetQuic ("ns3::QuicL4Protocol");
}

QuicHelper::~QuicHelper ()
{
}

void
QuicHelper::SetQuic (const std::string tid)
{
  m_quicFactory.SetTypeId (tid);
}

void
QuicHelper::Install (NodeContainer c) const
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
QuicHelper::InstallAll (void) const
{
  Install (NodeContainer::GetGlobal ());
}

void
QuicHelper::Install (Ptr<Node> node) const
{
  node->AggregateObject (m_quicFactory.Create<Object> ());
}

void
QuicHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

} // namespace ns3
