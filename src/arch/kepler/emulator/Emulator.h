/*
 *  Multi2Sim
 *  Copyright (C) 2014  Yuqing Shi (shi.yuq@husky.neu.edu)
 *
 *  This module is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This module is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this module; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ARCH_KEPLER_EMU_EMU_H
#define ARCH_KEPLER_EMU_EMU_H

#include <iostream>
#include <list>
#include <memory>
#include <vector>

#include <arch/common/Arch.h>
#include <arch/common/Emulator.h>
#include <arch/kepler/disassembler/Disassembler.h>
#include <lib/cpp/Debug.h>
#include <lib/cpp/Error.h>
#include <lib/cpp/Misc.h>
#include <memory/Memory.h>


namespace Kepler
{

class Disassembler;
class Grid;
class CUDADriver;
class ThreadBlock;
class Thread;
class Function;

class Emulator : public comm::Emulator
{
	// Debugger file
	static std::string isa_debug_file;

	// Simulation kind
	static comm::Arch::SimKind sim_kind;

	// Emu singleton instance
	static std::unique_ptr<Emulator> instance;

	// Disassembler
	Disassembler *disassembler;

	// List of Grids created by the guest application
	std::vector<std::unique_ptr<Grid>> grids;

	// List of Grid
	std::list<Grid *> pending_grids;
	std::list<Grid *> running_grids;
	std::list<Grid *> finished_grids;

	// Memory
	std::unique_ptr<mem::Memory> global_memory;
	std::unique_ptr<mem::Memory> constant_memory;
	std::unique_ptr<mem::Memory> shared_memory;

	// Field initialized in constructor. Global memory top address
	unsigned global_memory_top;

	// Field initialized in constructor. Global memory freed size
	unsigned global_memory_free_size;

	// Field initialized in constructor. Global memory total size
	unsigned global_memory_total_size;

	// Field initialized in constructor. Shared memory total size
	unsigned shared_memory_total_size;

	// Flags indicating whether the first 32 bytes of constant memory
	// are initialized. A warning will be issued by the simulator
	// if an uninitialized element is used by the kernel.
	bool const_mem_init[32];

	// Number of ALU instructions executed
	long long num_alu_instructions = 0;

	// Number of branch instructions executed
	long long num_branch_instructions = 0;

	// Number of shared memory instructions executed
	long long num_shared_mem_instructions = 0;

	// Number of global memory instructions executed
	long long num_global_memory_instructions = 0;

	/// Constructor
	Emulator();

public:

	/// Runtime error for Kepler
	class Error : public misc::Error
	{
	public:

		/// Constructor
		Error(const std::string &message) : misc::Error(message)
		{
			// Add module prefix
			AppendPrefix("Kepler emulator");
		}
	};

	/// Debugger
	static misc::Debug isa_debug;

	/// Kepler emulator maximum cycles
	long long max_cycles;

	/// Kepler emulator maximum number of instructions
	long long max_instructions;

	/// Kepler emulator maximum number of functions
	int max_functions;

	/// Warp size
	static const int warp_size = 32;

	/// Get the only instance of the Kepler emulator. If the instance does not
	/// exist yet, it will be created, and will remain allocated until the
	/// end of the execution.
	static Emulator *getInstance();

	/// Get grid list size
	unsigned getGridSize() { return grids.size(); }

	/// Get the assambler
	Disassembler *getAsm() const { return disassembler;}

	/// Get global memory top
	unsigned getGlobalMemTop() const { return global_memory_top; }

	/// Get global memory top
	unsigned* getGlobalMemTopAddress() { return &global_memory_top; }

	/// Get global memory free size
	unsigned getGlobalMemFreeSize() const { return global_memory_free_size; }

	/// Get global memory Total size
	unsigned getGlobalMemTotalSize() const { return global_memory_total_size; }

	/// Get ALU instruction count
	unsigned getNumAluInstructions() const { return num_alu_instructions; }

	/// Get Shared memory total size
	unsigned getSharedMemoryTotalSize() const
	{
		return shared_memory_total_size;
	}

	/// Get global memory
	mem::Memory* getGlobalMem() const  { return global_memory.get(); }

	/// Get constant memory
	mem::Memory * getConstMem() const { return constant_memory.get();}

	/// Return the number of available grids
	int getNumGrids() { return grids.size(); }

	/// Return the grid with the given identifier, or `nullptr` if the
	/// identifier does not correspond to a valid module.
	Grid *getGrid(int index)
	{
		return misc::inRange((unsigned) index, 0, grids.size()) ?
				grids[index].get() :
				nullptr;
	}

	/// Set global memory top
	void SetGlobalMemTop(unsigned value) { global_memory_top = value; }

	/// Set global memory free size
	void setGlobalMemFreeSize(unsigned value)
	{
		global_memory_free_size = value;
	}

	/// Set global memory total size
	void setGlobalMemTotalSize(unsigned value)
	{
		global_memory_total_size = value;
	}

	/// Increment ALU instruction counter
	void incNumAluInst() { num_alu_instructions ++; }

	/// Increse global memory top
	void incGloablMemTop(unsigned inc) { global_memory_top += inc; }

	/// Dump Kepler Emulator in a human-readable fashion into an output
	/// stream (or standard output if argument \a os is omitted.
	void Dump(std::ostream &os = std::cout) const;

	/// Dump emulator state (equivalent to Dump())
	friend std::ostream &operator<<(std::ostream &os, const Emulator &emu)
	{
		emu.Dump(os);
		return os;
	}

	/// Dump the statistics summary
	void DumpSummary(std::ostream &os);

	/// Run one iteration of the emulation loop
	bool Run();

	/// Write Constant Memory
	/// \param starting address to be written in
	/// \param size of data
	/// \param data buffer
	void WriteConstantMemory(unsigned addr, unsigned size, const char *buf);

	/// Write Global Memory
	/// \param starting address to be written in
	/// \param size of data
	/// \param data buffer
	void WriteGlobalMem(unsigned addr, unsigned size, const char *buf);

	/// Read Global Memory
	/// \param starting address to be read in
	/// \param size of data
	/// \param data buffer
	void ReadConstMem(unsigned addr, unsigned size, char *buf);

	/// Read Global Memory
	/// \param starting address to be read in
	/// \param size of data
	/// \param data buffer
	void ReadGlobalMem(unsigned addr, unsigned size, char *buf);

	/// Push an element into pending grid list
	void PushPendingGrid(Grid *grid);

	/// Create a new grid to the grid list and return a pointer to it.
	Grid *addGrid(Function *function);

	/// Register command-line options
	static void RegisterOptions();

	/// Process command-line options
	static void ProcessOptions();
};

}  //namespace Kepler

#endif
