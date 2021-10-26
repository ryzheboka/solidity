/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
// SPDX-License-Identifier: GPL-3.0
/** @file LinkerObject.cpp
 * @author Christian R <c@ethdev.com>
 * @date 2015
 */

#include <libevmasm/LinkerObject.h>
#include <libsolutil/CommonData.h>
#include <libsolutil/Keccak256.h>

using namespace std;
using namespace solidity;
using namespace solidity::util;
using namespace solidity::evmasm;

void LinkerObject::append(LinkerObject const& _other)
{
	for (auto const& ref: _other.linkReferences)
		linkReferences[ref.first + bytecode.size()] = ref.second;
	bytecode += _other.bytecode;
}

void LinkerObject::fillLinkReferences(std::string hexRepresentation)
{
	std::string foundPlaceholder;
	int const placeholderSize = 40; // 20 bytes or 40 hex characters
	linkReferences.clear();
	for (auto it = hexRepresentation.begin(); it != hexRepresentation.end();)
	{
		while (it != hexRepresentation.end() && *it != '_') ++it;
		if (it == hexRepresentation.end()) break;
		if (
			hexRepresentation.end() - it < placeholderSize ||
			*(it + 1) != '_' ||
			*(it + placeholderSize - 2) != '_' ||
			*(it + placeholderSize - 1) != '_'
		)

		foundPlaceholder = std::string(it, it + placeholderSize);
		linkReferences[static_cast<long unsigned int>((std::distance(hexRepresentation.begin(), it))/2)] = foundPlaceholder;
		it += placeholderSize;
	}
}

void LinkerObject::link(map<string, h160> const& _libraryAddresses)
{
	std::map<size_t, std::string> remainingRefs;
	for (auto const& linkRef: linkReferences)
		if (h160 const* address = matchLibrary(linkRef.second, _libraryAddresses))
			copy(address->data(), address->data() + 20, bytecode.begin() + vector<uint8_t>::difference_type(linkRef.first));
		else
			remainingRefs.insert(linkRef);
	linkReferences.swap(remainingRefs);
}

string LinkerObject::toHex() const
{
	string hex = solidity::util::toHex(bytecode);
	for (auto const& ref: linkReferences)
	{
		size_t pos = ref.first * 2;
		string hash = libraryPlaceholder(ref.second);
		hex[pos] = hex[pos + 1] = hex[pos + 38] = hex[pos + 39] = '_';
		for (size_t i = 0; i < 36; ++i)
			hex[pos + 2 + i] = hash.at(i);
	}
	return hex;
}

string LinkerObject::libraryPlaceholder(string const& _libraryName)
{
	return "$" + keccak256(_libraryName).hex().substr(0, 34) + "$";
}

h160 const*
LinkerObject::matchLibrary(
	string const& _linkRefName,
	map<string, h160> const& _libraryAddresses
)
{
	auto it = _libraryAddresses.find(_linkRefName);
	if (it != _libraryAddresses.end())
		return &it->second;
	return nullptr;
}
