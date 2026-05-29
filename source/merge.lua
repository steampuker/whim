#!/usr/bin/env lua
local newline = '\r\n'

local defines = {
    output = 'whim.h',
	out_path = '../',

    header = 'whim_interface.h',
    common = 'whim_impl.h',

    impl = {
        x11 = 'whim_x11.c'
    },

    license_notice = [[See the end of file for license information]],
    license = '../LICENSE'
}

--[[Utility functions]]

---@param str string
---@param start string
function string.startswith(str, start)
    return str:sub(1, #start) == start
end

---@param str string
---@param ending string
function string.endswith(str, ending)
    return ending == "" or str:sub(-#ending) == ending
end

---@param str string
---@param insert string
---@param pos integer
function string.insert(str, insert, pos)
    return str:sub(1, pos - 1) .. insert .. str:sub(pos)
end

---@param str string
function string.trim(str)
   return str:match'^()%s*$' and '' or str:match'^%s*(.*%S)'
end

---@param str string
---@param separator string
function string.split(str, separator)
   local sep, fields = separator or ':', {}
   local pattern = string.format('([^%s]+)', sep)
   str:gsub(pattern, function(c) fields[#fields+1] = c end)
   return fields
end

---@param str string
---@param old string
---@param new string
function string.replace(str, old, new)
    local search_start_idx = 1

    while true do
        local start_idx, end_idx = str:find(old, search_start_idx, true)
        if (not start_idx) then
            break
        end

        local postfix = str:sub(end_idx + 1)
        str = str:sub(1, (start_idx - 1)) .. new .. postfix

        search_start_idx = -1 * postfix:len()
    end

    return str
end

---@param path string
function os.makedir(path)
	assert((path:find(';') or path:find('|') == nil), "createDirectory(): Invalid path")

	os.execute("mkdir " .. path)
end

---@param path string
function os.exists(path)
	assert((path:find(';') or path:find('|') == nil), "pathExists(): Invalid path")

	local ok, err, code = os.rename(path, path)

	if (not ok) and (code == 13) then
		return true
	end

	return ok, err
end

--[[Amalgamation]]

--- @class Amalgamated
--- @field text string
--- @field cursor integer
local Amalgamated = { create = function () return {text = '', cursor = 0} end }

--- @param out Amalgamated
local function parseHeader(out)
	local f = assert(io.open(defines.header, 'r'), "parseHeader(): Header file not found")

	out.text = '// ' .. defines.license_notice:trim() .. newline
	out.text = out.text .. f:read('*a')

	f:close()

	out.cursor = assert(out.text:find('#endif', #out.text - 16))
end

--- @param out Amalgamated
local function parseCommon(out)
    local impl_header = '\n' .. '#ifdef WHIM_IMPLEMENTATION' .. newline
	out.text = out.text:insert(impl_header, out.cursor)
	out.cursor = out.cursor + #impl_header

	local f = assert(io.open(defines.common, 'r'), "parseCommon(): Common implementation not found")
    local common_text = f:read('*a')
    f:close()

    local match = '#include "' .. defines.header .. '"'
    local header_include = assert(common_text:find(match)) + #match

    common_text = common_text:sub(header_include):trim() .. newline .. newline
    out.text = out.text:insert(common_text, out.cursor)
    out.cursor = out.cursor + #common_text
end

--- @param out Amalgamated
local function parseX11(out)
	local f = assert(io.open(defines.impl.x11, 'r'), "parseX11(): X11 implementation not found")
    local x11_text = f:read('*a')
    f:close()

    local match = '#include "' .. defines.common .. '"'
    local header_include = assert(x11_text:find(match)) + #match

    x11_text = '// X11 Backend' .. newline .. x11_text:sub(header_include):trim() .. newline
    out.text = out.text:insert(x11_text, out.cursor)
    out.cursor = out.cursor + #x11_text
end

---@param out Amalgamated
local function parseFinish(out)
    out.text = out.text:insert('#endif' .. '\n\n', out.cursor)
end

---@param out Amalgamated
local function addLicense(out)
    local f = assert(io.open(defines.license, 'r'), "addLicense(): License file not found")
    local license_text = f:read('*a')
    f:close()

    local lines = license_text:split('\n')
    local license = ''

    local indent = '    '
    for _, v in ipairs(lines) do
        if #v > 1 then
            license = license .. indent .. v .. '\n'
        else
            license = license .. '\n'
        end
    end

    license = newline .. '/*' .. newline .. indent .. license:trim() .. newline .. '*/'

    out.text = out.text .. license
end

--[[Main]]

assert(defines.out_path:endswith('/'), "ERROR: out_path must end with /")
if (not defines == './') and
   (not defines == '../') and
   (not os.exists(defines.out_path)) then
	os.makedir(defines.out_path)
end

local out_file = assert(io.open(defines.out_path .. defines.output, 'w'), "ERROR: Could not create output file")
local out_text = Amalgamated:create()

parseHeader(out_text)
parseCommon(out_text)
parseX11(out_text)
parseFinish(out_text)

addLicense(out_text)

if(not out_text.text:endswith(newline)) then
	out_text.text = out_text.text .. newline
end

out_file:write(out_text.text)
out_file:close()
