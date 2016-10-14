This repository contains platforms maintained by the EDK II community.

# Introduction
Need place on tianocore.org where platforms can be maintained by the 
EDK II community.  This serves several purposes:

* Encourage more platforms sources to be shared earlier in the
  development process
* Allow platform sources to be shared that may not yet meet all edk2
  required quality criteria
* Allow platform source to be shared so the EDK II community may
  choose to help finish and validate
* Allow more platforms to be used as part of the edk2 validation and
  release cycle.

# Process for creating, using, and maintaining platforms
1. Create a new repo called edk2-platforms

    1. The default branch edk2-platforms/master contains all open 
       platforms that are actively validated against the packages
       in edk2/master.
   
    2. The intent is for packages in edk2-platforms to be CPU, Chipset,
       SoC, or platform specific.  Drivers that are CPU arch and platform
       agnostic should be put into the edk2 repo.
  
2. edk2-platforms discussions use the edk2-devel mailing list
   for design/patch/test using the following style for discussion
   of a platform branch in edk2-platforms repo.

     `[platforms/branch]: Subject`

3. All commits to edk2-platforms must follow same rules use for
   commits to edk2 (e.g. Tiano Contributor's Agreement)

4. Process to add a new branch to edk2-platforms

    1. Maintainer sends patch email to edk2-devel mailing list
       announcing the creation of a new branch in edk2-platforms 
       with Readme.md.  Readme.md must be in root of branch with 
       summary, owners, status, build instructions, target update
       instructions, OS compatibility, known issues/limitations, 
       links to related materials, and anything else a developer 
       needs to use platform(s) in that branch.  

    2. Readme.md must provide the PACKAGES_PATH setting required to 
       build along with the branch names of other repos that platform 
       requires.  This allows a platform developer(s) to use packages 
       from edk2/master or to use packages from a validated UDK release
       (e.g. edk2/UDK2015).
 
    3. Maintainer creates branch with Readme.md in edk2-platforms

    4. An edk2-platforms branch for platforms under development use the 
       following branch naming convention:

         `edk2-platforms/devel-*`

    5. An edk2-platforms branch for stable platforms use the following 
       branch naming convention:

         `edk2-platforms/stable-*`

5. Process to update sources in edk2-platforms branch

    1. Commit message subject format:

         `[PATCH][platforms/branch]: Package/Module: Subject`

    2. Directly commit changes to branch or if community review is desired,
       use edk2-devel review process.

6. Process to remove an edk2-platforms branch

    1. Stewards may periodically review of branches in edk2-platforms
       (once a quarter?)

    2. If no activity on a branch for extended period of time and the branch
       is not being maintained and is no longer functional then stewards 
       send email to edk2-devel to request deletion of edk2-platforms branch.

    3. If no objections from EDK II community, then branch is deleted and 
       archived at

         https://github.com/tianocore/edk2-archive
         
7. How to evaluate a platform in edk2-platforms

    1. Clone edk2-platforms/[branch name]

    2. Following instructions in Readme.md to build firmware and
       update target platform

# Multiple Platform Template

This section is a template for a Readme.md file in an edk2-platforms branch
that supports multiple platforms.  The list of platforms is presented here
with links to the Readme.md in each platform subdirectory.

* [Sample Platform](SamplePlatformPkg/Readme.md)
* [Another Platform](AnotherPlatformPkg/Readme.md)
* [Yet Another Platform](YetAnotherPlatformPkg/Readme.md)
