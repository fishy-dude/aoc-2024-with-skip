add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })
set_policy("run.autobuild", true)

add_requires("conan::ctre/3.9.0", { alias = "ctre", config = { settings = "compiler.cppstd=23" } })

rule("copy_txt")
    set_extensions(".txt")
    on_build_file(function (target, sourcefile)
        os.cp(sourcefile, path.join(target:targetdir(), path.basename(sourcefile) .. ".txt"))
    end)
rule_end()

for i = 1, 3 do
    local name = (i < 10 and "day0" or "day") .. tostring(i)
    target(name)
        set_kind("binary")
        set_languages("c++23")
        add_files("src/" .. name .. "/*.cpp", "src/" .. name .. "/*.txt")

        if i == 3 then 
            add_packages('ctre')
        end

        add_rules("mode.release", "mode.debug")
        add_rules("copy_txt")

        set_policy("build.warning", true)
        set_warnings("allextra", "pedantic", "error")
    target_end()
end
