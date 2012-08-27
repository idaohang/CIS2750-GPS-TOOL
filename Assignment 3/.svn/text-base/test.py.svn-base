#import sys
#from Tkinter import *
#makemodal = (len(sys.argv) > 1)

#def dialog():
    #win = Toplevel()                                     
    #Label(win,  text='Message!').pack()   
    #Button(win, text='OK', command=win.destroy).pack()   
    #if makemodal:
        #win.focus_set()       
        #win.grab_set()           
        #win.wait_window()       
    #print 'dialog exit'         

#root = Tk()
#Button(root, text='popup', command=dialog).pack()
#root.mainloop()

from Tkinter import *
root = Tk()

Label(root, text="Last Name:").grid(row=0, sticky=W)
Label(root, text="First Name:").grid(row=1, sticky=W)
Label(root, text="Phone #:").grid(row=2, sticky=W)
Label(root, text="email:").grid(row=3, sticky=W)
entry_ln = Entry(root)
entry_fn = Entry(root)
entry_ph = Entry(root)
entry_em = Entry(root)

entry_ln.grid(row=0, column=1)
entry_fn.grid(row=1, column=1)
entry_ph.grid(row=2, column=1)
entry_em.grid(row=3, column=1)
Label(root).grid(row=4, sticky=W)
Label(root, text="Skill set summary:").grid(row=5, sticky=W)
cb_gender = Checkbutton(root, text="Python")
cb_gender.grid(row=6, sticky=W)
cb_gender = Checkbutton(root, text="Perl")
cb_gender.grid(row=7, sticky=W)
b_apply = Button(root, text="Apply")
b_apply.grid(row=7, column = 1)
root.mainloop()