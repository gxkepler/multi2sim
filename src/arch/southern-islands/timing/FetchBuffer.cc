/*
 *  Multi2Sim
 *  Copyright (C) 2015  Rafael Ubal (ubal@ece.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cassert>

#include "FetchBuffer.h"
#include "ComputeUnit.h"


namespace SI
{

FetchBuffer::FetchBuffer(int id, ComputeUnit *compute_unit) :
					id(id),
					compute_unit(compute_unit)
			{
				// Initialize the uop buffer
				buffer.resize(compute_unit->fetch_buffer_size * compute_unit->fetch_width);

				// Initialize the last fetched warp index
				last_fetched_wavefront_index = -1;

				// Initialize the last dispatched warp index
				last_issued_wavefront_index = -1;
			}

void FetchBuffer::Remove(std::vector<std::unique_ptr<Uop>>::iterator it)
{
	assert(it != buffer.end());
	buffer.erase(it);
}

}

