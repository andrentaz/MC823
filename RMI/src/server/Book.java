package server;

import java.util.ArrayList;

public class Book {
	/* Attributes */
	private String isbn, pubHouse;
	private String desc, title;
	private int store, relDate;
	private ArrayList<String> author;
	
	
	/* Methods */
	public String getIsbn() {
		return isbn;
	}
	public void setIsbn(String isbn) {
		this.isbn = isbn;
	}


	public String getPubHouse() {
		return pubHouse;
	}
	public void setPubHouse(String pubHouse) {
		this.pubHouse = pubHouse;
	}

	
	public String getDesc() {
		return desc;
	}
	public void setDesc(String desc) {
		this.desc = desc;
	}


	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = title;
	}


	public int getRelDate() {
		return relDate;
	}
	public void setRelDate(int relDate) {
		this.relDate = relDate;
	}


	public int getStore() {
		return store;
	}
	public void setStore(int store) {
		this.store = store;
	}
	
	
	public ArrayList<String> getAuthor() {
		return author;
	}
	public void setAuthor(ArrayList<String> author) {
		this.author = author;
	}	

}	/* End Book */