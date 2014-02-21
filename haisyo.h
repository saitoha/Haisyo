/*****************************************************************************
 * Copyright 2005-2014  Hayaki Saito <user@zuse.jp>
 *
 * ##### BEGIN GPL LICENSE BLOCK #####
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ##### END GPL LICENSE BLOCK #####
 *
 *       haisyo.h
 *
 *                          ver.0.0    2005/09/24   Hayaki Saito
 *                          ver.0.1    2005/09/25   Hayaki Saito
 *                          ver.0.2    2012/09/02   Hayaki Saito
 *
 *******************************************************************************/

#if !defined(__HOOK_H__)
#define __HOOK_H__ 1

#define EXPORT __declspec(dllexport) 

EXPORT BOOL CALLBACK 
SetHaisyoHook(void); 

EXPORT BOOL CALLBACK 
UnsetHaisyoHook(void); 

#endif /* __HOOK_H__ */

