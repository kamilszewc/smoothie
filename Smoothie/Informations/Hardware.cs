using System;
using System.Management;

namespace Smoothie.Informations
{
    class Hardware
    {
        ManagementObjectCollection _cpus;
        ManagementObjectCollection _gpus;
        ManagementObjectCollection _memory;

        public Hardware()
        {
            ManagementObjectSearcher cpuSearcher = new ManagementObjectSearcher("select * from Win32_Processor");
            ManagementObjectSearcher memSearcher = new ManagementObjectSearcher("select * from Win32_PhysicalMemory");
            ManagementObjectSearcher gpuSearcher = new ManagementObjectSearcher("select * from Win32_VideoController");

            _cpus = cpuSearcher.Get();
            _memory = memSearcher.Get();
            _gpus = gpuSearcher.Get();
        }

        public string GetCPUs()
        {
            string cpusInfo = "";

            foreach (ManagementObject cpu in _cpus)
            {
                PropertyDataCollection properties = cpu.Properties;
                cpusInfo += properties["Name"].Value + "\n";
                cpusInfo += "Number of cores: " + properties["NumberOfCores"].Value + "\n";
                cpusInfo += "Number of logical processors: " + properties["NumberOfLogicalProcessors"].Value + "\n";
                cpusInfo += "Max clock speed: " + properties["MaxClockSpeed"].Value + "MHz" + "\n\n";
            }

            foreach (ManagementObject mem in _memory)
            {
                PropertyDataCollection properties = mem.Properties;
                UInt64 memCapacity = Convert.ToUInt64(properties["Capacity"].Value);
                cpusInfo += "Memory capacity: " + Convert.ToString(memCapacity / (1024*1024)) + "MB" + "\n";
                cpusInfo += "Memory frequency: " + properties["Speed"].Value + "MHz" + "\n";
            }
            
            return cpusInfo;
        }

        public int GetTotalNumberOfLogicalProcessors()
        {
            int totalNumberOfLogicalProcessors = 0;
            foreach (ManagementObject cpu in _cpus)
            {
                PropertyDataCollection properties = cpu.Properties;
                totalNumberOfLogicalProcessors += Convert.ToInt32(properties["NumberOfLogicalProcessors"].Value);
            }

            return totalNumberOfLogicalProcessors;
        }

        public string GetGPUs()
        {
            string gpusInfo = "";

            foreach (ManagementObject gpu in _gpus)
            {
                PropertyDataCollection properties = gpu.Properties;
                gpusInfo += properties["VideoProcessor"].Value + "\n";
                gpusInfo += "Driver version: " + properties["DriverVersion"].Value + "\n";
                UInt64 memCapacity = Convert.ToUInt64(properties["AdapterRAM"].Value);
                gpusInfo += "Device memory: " + Convert.ToString(memCapacity / (1024 * 1024)) + "MB" + "\n";
                gpusInfo += "No CUDA compatibility" + "\n";
            }

            return gpusInfo;
        }
    }
}
