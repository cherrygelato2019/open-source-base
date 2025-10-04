#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <vector>

struct driver_t {
    DWORD process_id = 0;
    HANDLE process_handle = nullptr;

    bool attach(const std::wstring& proc_name) {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(entry);

        if (Process32FirstW(snap, &entry)) {
            do {
                if (!_wcsicmp(entry.szExeFile, proc_name.c_str())) {
                    process_id = entry.th32ProcessID;
                    CloseHandle(snap);

                    process_handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, process_id);
                    return process_handle != nullptr;
                }
            } while (Process32NextW(snap, &entry));
        }
        CloseHandle(snap);
        return false;
    }

    uintptr_t get_module_base(const std::wstring& mod_name) {
        if (!process_handle) return 0;

        HMODULE hMods[1024];
        DWORD needed;
        if (EnumProcessModules(process_handle, hMods, sizeof(hMods), &needed)) {
            size_t count = needed / sizeof(HMODULE);
            for (size_t i = 0; i < count; i++) {
                wchar_t modName[MAX_PATH];
                if (GetModuleBaseNameW(process_handle, hMods[i], modName, MAX_PATH)) {
                    if (!_wcsicmp(modName, mod_name.c_str())) {
                        return reinterpret_cast<uintptr_t>(hMods[i]);
                    }
                }
            }
        }
        return 0;
    }

    template <typename T>
    T read(uintptr_t addr) {
        T buffer{};
        if (!process_handle) return buffer;
        ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(addr), &buffer, sizeof(T), nullptr);
        return buffer;
    }

    template <typename T>
    bool write(uintptr_t addr, const T& value) {
        if (!process_handle) return false;
        return WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), nullptr) != 0;
    }
    template<typename T>
    bool safe_read(std::uint64_t Address, T* Out, SIZE_T Size = sizeof(T)) {
        if (!process_handle || !Out || !Address || Size == 0 || Size > sizeof(T) * 8 || reinterpret_cast<void*>(Address) == nullptr) return false;
        __try {
            SIZE_T BytesRead = 0;
            if (!ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(Address), Out, Size, &BytesRead)) return false;
            return BytesRead == Size;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }

    std::string ReadString(std::uint64_t Address)
    {
        constexpr int MaxSafeLength = 200;

        int Length = 0;
        if (!safe_read(Address + 0x18, &Length, sizeof(Length)) || Length <= 0 || Length > MaxSafeLength) {
            return "";
        }

        std::uint64_t StringAddress = Address;
        if (Length >= 16) {
            if (!safe_read(Address, &StringAddress, sizeof(StringAddress))) {
                return "";
            }
        }

        std::vector<char> Buffer(static_cast<size_t>(Length) + 1, '\0');
        SIZE_T BytesRead = 0;
        if (!ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(StringAddress), Buffer.data(), Length, &BytesRead) || BytesRead != static_cast<SIZE_T>(Length)) {
            BytesRead = 0;
            for (int I = 0; I < Length; ++I) {
                char C;
                if (safe_read(StringAddress + I, &C, sizeof(char))) {
                    Buffer[I] = C;
                    ++BytesRead;
                }
            }
            Buffer[BytesRead] = '\0';
        }

        return std::string(Buffer.data());
    }



    void detach() {
        if (process_handle) {
            CloseHandle(process_handle);
            process_handle = nullptr;
        }
        process_id = 0;
    }
};
inline driver_t driver;