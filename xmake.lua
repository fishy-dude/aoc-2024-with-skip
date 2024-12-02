add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })
set_policy("run.autobuild", true)

rule("copy_files_to_build")
    on_build(function (target)
        os.cp("$(projectdir)/src/" .. target:name() .. "/input.txt", target:targetdir() .. "/input.txt")
    end)

for i = 1, 1 do
    local name = (i < 10 and "day0" or "day") .. tostring(i)
    target(name)
        set_kind("binary")
        set_languages("c++23")
        add_files("src/" .. name .. "/*.cpp")
        set_policy("build.c++.modules", true)

        add_rules("mode.release", "mode.debug")
        add_rules("copy_files_to_build")

        set_policy("build.warning", true)
        set_warnings("allextra", "pedantic", "error")
    target_end()
end
