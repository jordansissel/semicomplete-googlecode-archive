use Gtk2::SimpleMenu;

sub create_menubar ()
{
	my $menu_tree = [
		_File => {
			item_type => '<Branch>',
			children => [
				"Create _Folder" => {
					item_type => '<StockItem>',
					callback => \&mkdir_cb,
					callback_action => 0,
					extra_data => 'gtk-open',
				},
				"Change P_ermissions" => {
					item_type => '<StockItem>',
					callback => \&chmod_cb,
					callback_action => 1,
					extra_data => 'gtk-execute',
				},
			],
		},
		_Edit => {
			item_type => '<Branch>',
			children => [
				"Cu_t Files" => {
					item_type => '<StockItem>',
					callback => \&cut_cb,
					callback_action => 2,
					extra_data => 'gtk-cut',
				},
				"_Copy Files" => {
					item_type => '<StockItem>',
					callback => \&copy_cb,
					callback_action => 3,
					extra_data => 'gtk-copy',
				},
				"_Paste Files" => {
					item_type => '<StockItem>',
					callback => \&paste_cb,
					callback_action => 4,
					extra_data => 'gtk-paste',
				},
				Separator => {
					item_type => '<Separator>',
				},
				"_Delete Files" => {
					item_type => '<StockItem>',
					callback => \&delete_cb,
					callback_action => 5,
					extra_data => 'gtk-delete',
				},
			],
		},
		_Help => {
			item_type => '<Branch>',
			children => [
				"_About" => {
					item_type => '<StockItem>',
					callback => \&about_cb,
					callback_action => 6,
					extra_data => 'gtk-dialog-warning',
				},
			],
		},
	];
   	                                                                         
	my $menubar = Gtk2::SimpleMenu->new (
		menu_tree => $menu_tree,
		default_callback => \&default_callback,
		user_data => 'user_data',
	);
                                                                            
	return $menubar->{widget};
}

1;
