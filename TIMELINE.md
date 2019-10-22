# Timeline

## Milestones by category

1. `ptrace`:
   - [x] `qzh` fork & ptrace & exec
   - [x] `qzh` Intercept parameters and send to parser
   - [ ] `qzh` Cancel syscall
   - [ ] `qzh` Change syscall parameters
1. argument parsing:
   - [ ] `qzh` Core syscalls: `open`, `access`, `clone`, `fork`/`vfork`, `execve`, `socket`
   - [ ] `qzh` More syscalls: `stat`, `lstat`, `brk`
   - [ ] `qzh` Network syscalls: `bind`, `listen`, `connect`, `recvfrom`, `sendto`, etc
   - [ ] `qzh` Filesystem syscalls: `link`, `unlink`, `truncate`, etc
1. ui:
   - [x] `lanw3` Display arguments
   - [ ] `lanw3` Let user choose yes/no
   - [ ] `lanw3` Let user change arguments
   - [x] `lanw3` Add password protection
   - [ ] `lanw3` Optional: gui interfaces
1. config file:
   - [ ] `pcgubi98` Define rule/config file format
   - [ ] `pcgubi98` Parse rules from config file
   - [ ] `pcgubi98` Encrypting the config file
   - [ ] `lanw3` Optional: modify config file by commands
1. misc:
   - [ ] `pcgubi98` Logging
   - [ ] `pcgubi98` `lanw3` Write test programs (normal and adversarial)
   - [ ] `pcgubi98` `lanw3` Test on common programs (such as chrome, etc.)
   - [ ] `qzh` Optional: Benchmark and graph (qzh)

## Timeline (by Tuesday each week, before meeting)

### Week 4:

- [x] `qzh` general (build system) setup
- [x] `qzh` 1.a
- [x] `qzh` 1.b
- [ ] `qzh` 2.a
- [x] `lanw3` 3.a
- [x] `lanw3` 3.d

### Week 5:

- [ ] `qzh` 1.c
- [ ] `qzh` 2.b
- [ ] `pcgubi98` 5.a
- [ ] `lanw3` 3.b
- [ ] `lanw3` 3.c

### Week 6 (Prototype):

- [ ] `pcgubi98` 4.a
- [ ] `pcgubi98` 4.b
- [ ] `qzh` 2.c
- [ ] `lanw3` 4.d

### Week 7 (feature complete):

- [ ] `pcgubi98` `lanw3` 5.b
- [ ] `pcgubi98` `lanw3` 5.c
- [ ] `qzh` 2.d

### Week 8-10:

- [ ] `qzh` 5.d
- [ ] `*` Refinement
