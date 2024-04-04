#!python
import time

profile_array = []

profile_array.append("Debug")
profile_array.append("Release")
profile_array.append("Debug_BSUR")
profile_array.append("Debug_BAUR")
profile_array.append("Debug_BQUR")
profile_array.append("Debug_BQFR")
profile_array.append("Debug_WQUR")
profile_array.append("Debug_WQFR")
profile_array.append("Release_BSUR")
profile_array.append("Release_BAUR")
profile_array.append("Release_BQUR")
profile_array.append("Release_BQFR")
profile_array.append("Release_WQUR")
profile_array.append("Release_WQFR")
profile_count = len(profile_array)

format_info = "{:2}. {:7}: {:30}"
format_switching = "    {:15}:{:4}"
format_building  = "   {:15}:{:4}"

print(" ")
print("{0}: {1}".format("Number of build profile(s): ",profile_count))
print("Saving ... "),
Save()
print(" ")

for i in range (0, profile_count):
    print(format_info.format(i, "Profile", profile_array[i]))
    print (format_switching.format("Switching","...")),
    switch_status = build.ChangeBuildMode(profile_array[i])
    print (format_building.format("Building","...")),
    build_status = build.All(False, True)
    
    if build_status == False:
        print ("Build end: Have error at build profile {0}".format(profile_array[i]))
        break

