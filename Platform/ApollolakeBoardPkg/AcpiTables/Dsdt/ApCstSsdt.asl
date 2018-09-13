DefinitionBlock (
  "APCST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApCst",
  0x3000
  )
{
  External(\_PR.CPU1, DeviceObj)
  External(\_PR.CPU2, DeviceObj)
  External(\_PR.CPU3, DeviceObj)
  External(\_PR.CPU0._CST)

  include ("ApCst.asl")
}
